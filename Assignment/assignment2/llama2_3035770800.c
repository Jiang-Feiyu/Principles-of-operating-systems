/*
PLEASE WRITE DOWN FOLLOWING INFO BEFORE SUBMISSION
* FILE NAME: llama2_3035770800.c
* NAME: Jiang Feiyu
* UID : 3035770800
* Development Platform: Vscode (SSH: workbrench2)
* Remark: (How much you implemented?)
* How to compile: (gcc -o llama2_[UID] llama2_[UID].c utilities.c -O2 -pthread -lm)

Please download the model and tokenizer to the same folder:
$ wget -O model.bin https://huggingface.co/huangs0/llama2.c/resolve/main/model.bin
$ wget -O tokenizer.bin https://huggingface.co/huangs0/llama2.c/resolve/main/tokenizer.bin

In compile, remember to add `-pthred` to link library:
$ gcc -o llama2_[UID] llama2_[UID].c utilities.c -O2 -pthread -lm

Then Run with:
$ ./llama2_[UID] <seed> <thr_count>
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "utilities.h"

/**
 * ----------------------------------------------------------------------------
 * TASK - Optimize Matrix-Vector Multiplication by Multi-Threading
 * 
 * Matrix-Vector Multiplication, used in Attention and Feed-Forward Network
 * is the most time-consuming part of GPT. Luckily, most of computation is 
 * independent of each row, so we can use Multi-Threading for acceleration.
 * 
 * Please use <pthread.h> and your favorite synchronization method,
 * semaphore / mutex lock + conditional variable
 * 
 * A sequential version is provided in seq.c, please modify it to parallel version.
*/

// YOUR CODE STARTS HERE

#include <pthread.h>
#include <unistd.h>

// Global Variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
// 
struct rusage main_usage;
int thread_count;

// Thread-specific variables
typedef struct {
    float* out;
    float* vec;
    float* mat;
    int col;
    int row;
} ThreadArgs;

void *thr_func(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    float* out = args->out;
    float* vec = args->vec;
    float* mat = args->mat;
    int col = args->col;
    int row = args->row;

    // Fall asleep immediately after initialization
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&condition, &mutex);
    pthread_mutex_unlock(&mutex);

    // Calculation logic goes here

    // Inform main thread after finishing the task
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);

    // Collect system usage (of itself)
    struct rusage thread_usage;
    getrusage(RUSAGE_THREAD, &thread_usage);

    // Terminate the thread
    pthread_exit(NULL);
}

int init_mat_vec_mul(int thr_count) {
    thread_count = thr_count;  // No. of threads

    pthread_t threads[thread_count];
    int thread_ids[thread_count];

    for (int i = 0; i < thread_count; i++) {
        thread_ids[i] = i;  // Let threads identify themselves
        pthread_create(&threads[i], NULL, thr_func, &thread_ids[i]);  // create thread
    }

    sleep(0);  // let the threads fall asleep immediately

    return 0;
}

void mat_vec_mul(float* out, float* vec, float* mat, int col, int row) {
    // Initialize thread arguments
    ThreadArgs thread_args[thread_count];

    // Assign parameters to thread arguments
    for (int i = 0; i < thread_count; i++) {
        thread_args[i].out = out + (i * row);
        thread_args[i].vec = vec;
        thread_args[i].mat = mat + (i * col);
        thread_args[i].col = col;
        thread_args[i].row = row;
    }

    // Acquire mutex lock
    pthread_mutex_lock(&mutex);

    // Signal threads to start calculation
    pthread_cond_broadcast(&condition);

    // Wait for threads to finish calculation
    for (int i = 0; i < thread_count; i++) {
        pthread_cond_wait(&condition, &mutex);
    }

    // Release mutex lock
    pthread_mutex_unlock(&mutex);
}

int close_mat_vec_mul() {
    // Signal threads to finish their tasks
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&condition);
    pthread_mutex_unlock(&mutex);

    // Wait for all threads to finish
    pthread_t threads[thread_count];
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy mutex and condition variables
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return 0;
}



// YOUR CODE ENDS HERE

int transformer(int token, int pos, LLMConfig* p, LLMRuntime* s, LLMWeight* w) {
    
    // a few convenience variables
    int dim = p->dim, hidden_dim =  p->hidden_dim, head_size = p->dim / p->n_heads;

    // copy the token embedding into x
    memcpy(s->x, &(w->token_embedding_table[token * dim]), dim*sizeof(float));

    // forward all the layers
    for(int l = 0; l < p->n_layers; l++) {

        // Attention
        {
            // attention normalization
            normalize(s->xb, s->x, w->rms_att_weight + l*dim, dim);

            // q, k, v = w_q @ x, w_k @ x, w_v @ x, respectively
            mat_vec_mul(s->q, s->xb, w->wq + l*dim*dim, dim, dim);
            mat_vec_mul(s->k, s->xb, w->wk + l*dim*dim, dim, dim);
            mat_vec_mul(s->v, s->xb, w->wv + l*dim*dim, dim, dim);

            // apply positional embedding
            position_embedding(s->q, s->k, w, pos, p->dim, p->n_heads);

            // save intermediate result for later reference
            key_value_cache(l, pos, p, s);
            
            // attention calculation
            attention(l, pos, p, s, w);

            // wo @ x to get final result
            mat_vec_mul(s->xb2, s->xb, w->wo + l*dim*dim, dim, dim);

            // residual connection back into x
            accum(s->x, s->xb2, dim);
        }
    
        // Feed-Forward Network: w2 @ (silu(w1 @ x) * (w3 @ x)), * is element-wise multiply
        {
            // FFN Normalization
            normalize(s->xb, s->x, w->rms_ffn_weight + l*dim, dim);

            // w1 @ x
            mat_vec_mul(s->h1, s->xb, w->w1 + l*dim*hidden_dim, dim, hidden_dim);
            // silu(w1 @ x)
            silu(s->h1, hidden_dim);
            // w3 @ x
            mat_vec_mul(s->h2, s->xb, w->w3 + l*dim*hidden_dim, dim, hidden_dim);
            // silu(w1 @ x) * (w3 @ x)
            element_wise_mul(s->h1, s->h2, hidden_dim);
            // w2 @ (silu(w1 @ x) * (w3 @ x))
            mat_vec_mul(s->xb, s->h1, w->w2 + l*dim*hidden_dim, hidden_dim, dim);

            // residual connection
            accum(s->x, s->xb, dim);
        }
    }
    
    // final normalization
    normalize(s->x, s->x, w->rms_final_weight, dim);
    // classifier into logits
    mat_vec_mul(s->logits, s->x, w->token_embedding_table, p->dim, p->vocab_size);
    // apply the temperature to the logits
    for (int q=0; q<p->vocab_size; q++) { s->logits[q] /= 0.9f; }
    // apply softmax to the logits to get the probabilities for next token
    softmax(s->logits, p->vocab_size);
    // now sample from this distribution to get the next token
    return sample(s->logits, p->vocab_size);
}

int main(int argc, char* argv[]) {

    unsigned int seed;
    int thr_count;

    // Get the seed value
    if (argc == 3) {
        seed = atoi(argv[1]);
        thr_count = atoi(argv[2]);
    } else {
        printf("Usage: ./compiled <seed> <thr_count>\n");
        return 1;
    }

    // Initialize
    srand(seed);
    init_mat_vec_mul(thr_count);

    // load model
    LLMConfig config;
    LLMWeight weights;
    if (load_LLM_Config_Weight(&config, &weights) == 1) { return 1; }

    // load tokenizer
    char** vocab = malloc(config.vocab_size * sizeof(char*));
    if (load_tokenizer(vocab, config.vocab_size) == 1) { return 1; }

    // create and init the application LLMRuntime
    LLMRuntime state; 
    malloc_LLMRuntime(&state, &config);
    
    // the current position we are in
    long start = time_in_ms();

    int next, token = 1, pos = 0; // token = 1 -> <START>
    
    while (pos < config.seq_len) {

        // forward the transformer to get logits for the next token
        next = transformer(token, pos, &config, &state, &weights);

        printf("%s", vocab[next]);
        fflush(stdout); // force print

        token = next;
        pos++;
    }

    long end = time_in_ms();
    printf("\n\nlength: %d, time: %f s, achieved tok/s: %f\n", config.seq_len, (double)(end-start)/1000, config.seq_len / (double)(end-start)*1000);

    // cleanup
    close_mat_vec_mul();
    free_LLMRuntime(&state);
    free_LLMWeight(&weights);
    for (int i = 0; i < config.vocab_size; i++) { free(vocab[i]); }
    free(vocab);
    return 0;
}
