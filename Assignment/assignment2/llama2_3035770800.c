/*
PLEASE WRITE DOWN FOLLOWING INFO BEFORE SUBMISSION
* FILE NAME: llama2_3035770800.c
* NAME: Jiang Feiyu
* UID : 3035770800
* Development Platform: Vscode (SSH: workbrench2)
* Remark: (How much you implemented?)
*   - Documentation - finished
*   - Report - finished
*   - Implementation
*       - Correct result & multi-threading
*       - Achieve > 10% acceleration with sequential by multi-threads
*       - Reuse threads in multi-threading
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
#include <time.h>

// Global Variables
struct rusage main_usage; // get usage for main thread
int thread_count;         // Number of threads
pthread_t *threads;       // Thread handles
pthread_cond_t *con;      // Condition variables for each thread
pthread_mutex_t *mutex;   // Mutexes for each thread
int terminate = 0;        // Flag indicating when threads should exit

// Structure to hold thread arguments
struct thread_data
{
    pthread_t thread_id; 
    float *out; // Pointer to the output vector
    float *vec;
    float *mat;      // Pointer to the matrix
    int col;         // Number of columns in the matrix
    int row;         // Number of rows in the matrix
    int *work_start; // Flag to indicate if the thread has started its work
    int *work_done;  // Flag to indicate if the thread has finished its work
    struct rusage usage;
    clockid_t clock_id; // Thread-specific clock id
};
struct thread_data *thread_datas;

/*
 * Thread function for matrix-vector multiplication.
 *
 * This function represents the behavior of each thread in the parallel matrix-vector
 * multiplication. It is responsible for computing a subset of rows assigned to the
 * thread and updating the output vector. The thread can be controlled by the main thread,
 * and it can collect its own system usage before termination.
 *
 * Parameters:
 *   - arg: Thread ID passed as an intptr_t argument.
 *
 * Behavior:
 * a. Falls asleep immediately after initialization.
 * b. Can be woken up by the main thread to work on assigned tasks.
 * c. After finishing the task, informs the main thread.
 * d. Is able to collect its system usage and terminate when instructed.
 */
void *thr_func(void *arg)
{
    intptr_t id = (intptr_t)arg;  // Cast argument to intptr_t to get the thread ID
    int start_row, end_row;
    while (1)
    {
        pthread_mutex_lock(&mutex[id]);
        while (!*(thread_datas[id].work_start) && !terminate)
        {
            pthread_cond_wait(&con[id], &mutex[id]);
        }
        if (terminate)
        {
            pthread_mutex_unlock(&mutex[id]);
            break; // Exit the thread loop if the termination flag is set
        }
        // Compute the number of rows per thread each time work is started
        int rows_per_thread = thread_datas[id].row / thread_count; // Compute the number of rows per thread
        int extra_rows = thread_datas[id].row % thread_count;      // Compute the remaining rows
        // Determine start and end rows for this thread
        start_row = id * rows_per_thread;
        end_row = (id + 1) * rows_per_thread - 1;
        if (id == thread_count - 1)
        { // Last thread takes any extra rows
            end_row += extra_rows;
        }
        *(thread_datas[id].work_start) = 0; // Reset the work_start flag
        pthread_mutex_unlock(&mutex[id]);
        // Perform matrix-vector multiplication for the assigned rows
        // GPT code start from here
        for (int i = start_row; i <= end_row; i++)
        {
            float result = 0.0f;
            for (int j = 0; j < thread_datas[id].col; j++)
            {
                result += thread_datas[id].mat[i * thread_datas[id].col + j] * thread_datas[id].vec[j];
            }
            thread_datas[id].out[i] = result;
        }
        getrusage(RUSAGE_SELF, &thread_datas[id].usage);
        // GPT code end here
        // Signal that this thread's work is done
        pthread_mutex_lock(&mutex[id]);
        *(thread_datas[id].work_done) = 1;
        pthread_cond_signal(&con[id]);
        pthread_mutex_unlock(&mutex[id]);
    }
    return NULL;
}

/*
 * a. Create n threads
 * b. Let threads identify themselves, i.e., each thread knows it is the i-th threads
 * c. Let the created threads fall asleep immediately
*/
int init_mat_vec_mul(int thr_count) {
    thread_count = thr_count;  // No. of threads
    // Allocate memory for thread data sturctures
    threads = malloc(thread_count * sizeof(pthread_t));
    con = malloc(thread_count * sizeof(pthread_cond_t));
    mutex = malloc(thread_count * sizeof(pthread_mutex_t));
    thread_datas = malloc(thread_count * sizeof(struct thread_data));

    // Initialize and create the specified amount of threads
    for (int i = 0; i < thread_count; i++) {
        // Initialize condition variables and mutex
        pthread_condattr_t cond_attr;
        pthread_condattr_init(&cond_attr);
        pthread_cond_init(&con[i], &cond_attr);
        pthread_mutexattr_t mutex_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_mutex_init(&mutex[i], &mutex_attr);
        // Set the parameters of the thread data structure and initialize it according to the specific requirements
        thread_datas[i].out = NULL;
        thread_datas[i].vec = NULL; 
        thread_datas[i].mat = NULL;
        thread_datas[i].col = 0;
        thread_datas[i].row = 0; 
        thread_datas[i].work_start = malloc(sizeof(int)); // Allocate memory for work_start
        thread_datas[i].work_done = malloc(sizeof(int));  // Allocate memory for work_done
        *(thread_datas[i].work_start) = 0;  // Initialize work_start to 0
        *(thread_datas[i].work_done) = 0;   // Initialize work_done to 0

        // Create a thread and pass the address of the thread data structure as an argument
        intptr_t thread_id = (intptr_t)i;
        pthread_create(&threads[i], NULL, thr_func, (void*)thread_id); 
    }

    sleep(0); // Threads will fall asleep immediately

    return 0;
}

/*
 * a. Assign new parameters (out, vec, mat, col, row) to threads
 * b. Wake up threads to do calculation
 * c. Main thread wait until all threads finished task, and then return
 * Wait for all threads to finish their work and reset the work_done flags.
 *
 * Parameters:
 *   - thread_datas: Array of thread data structures.
 *   - mutex: Array of mutexes for synchronization.
 *   - con: Array of condition variables for signaling.
 *   - thread_count: Number of threads.
 */
void wait_for_threads(struct thread_data *thread_datas, pthread_mutex_t *mutex, pthread_cond_t *con, int thread_count)
{
    for (int i = 0; i < thread_count; i++)
    {
        pthread_mutex_lock(&mutex[i]);
        // Wait until the thread signals that it has finished its work
        while (!*(thread_datas[i].work_done))
        {
            pthread_cond_wait(&con[i], &mutex[i]);
        }
        // Reset the work_done flag after the thread has finished its work
        *(thread_datas[i].work_done) = 0;
        pthread_mutex_unlock(&mutex[i]);
    }
}

/*
 * Perform matrix-vector multiplication, optionally using multi-threading.
 *
 * If thread_count is 0, perform matrix-vector multiplication sequentially.
 * Otherwise, assign new parameters to threads and signal them to start work.
 * Wait for all threads to finish before returning.
 *
 * Parameters:
 *   - out: Pointer to the output vector.
 *   - vec: Pointer to the input vector.
 *   - mat: Pointer to the matrix.
 *   - col: Number of columns in the matrix.
 *   - row: Number of rows in the matrix.
 */
void mat_vec_mul(float *out, float *vec, float *mat, int col, int row)
{
    terminate = 0;  // Reset terminate flag at the beginning of the function
    //printf("mat_vec_mul start\n");

    // If thread_count is 0, perform matrix-vector multiplication sequentially
    if (thread_count == 0)
    {
        for (int i = 0; i < row; i++)
        {
            float val = 0.0f;
            // Compute the dot product of the i-th row of the matrix and the input vector
            for (int j = 0; j < col; j++)
            {
                val += mat[i * col + j] * vec[j];
            }
            // Assign the result to the output vector
            out[i] = val;
        }
        return;
    }

    // Assign new parameters to threads and signal them to start work
    for (int i = 0; i < thread_count; i++)
    {
        pthread_mutex_lock(&mutex[i]);
        // Set thread parameters
        thread_datas[i].out = out;
        thread_datas[i].vec = vec;
        thread_datas[i].mat = mat;
        thread_datas[i].col = col;
        thread_datas[i].row = row;
        // Reset the work_done flag and signal the thread to start work
        *(thread_datas[i].work_done) = 0;
        *(thread_datas[i].work_start) = 1;
        pthread_cond_signal(&con[i]);
        pthread_mutex_unlock(&mutex[i]);
    }

    // Wait for all threads to finish
    wait_for_threads(thread_datas, mutex, con, thread_count);
}

/*
 * a. Wake up threads to collect the system usage (of themselves) and terminates
 * b. Wait until all threads to exit and collect system usage of threads
 * c. Collect system usage of main thread, and display both usage of each thread and main thread
 * d. Clear all resources related with multi-threading, and return
 * Wait for all threads to complete their tasks and reset the work_done flags.
*/
int close_mat_vec_mul() {
    for (int i = 0; i < thread_count; ++i) {
        // Signal all threads to terminate.
        terminate = 1; // Set terminate flag to a non-zero value
        pthread_mutex_lock(&mutex[i]);
        pthread_cond_signal(&con[i]); // Wake up the thread
        pthread_mutex_unlock(&mutex[i]);
        // Print resource usage of each thread
        // GPT code start here
        printf("Thread %d has completed - user: %ld.%06ld s, system: %ld.%06ld s\n",
               i,
               (long)thread_datas[i].usage.ru_utime.tv_sec, (long)thread_datas[i].usage.ru_utime.tv_usec,
               (long)thread_datas[i].usage.ru_stime.tv_sec, (long)thread_datas[i].usage.ru_stime.tv_usec);
        // GPT code end here
        // Join all threads to make sure they have finished.
        pthread_join(threads[i], NULL);
        // Clean up resources.
        pthread_mutex_destroy(&mutex[i]);    
        free(thread_datas[i].work_start);
        free(thread_datas[i].work_done);
    }
    // Get and print the main thread's resource usage.
    getrusage(RUSAGE_SELF, &main_usage);
    // GPT code start here
    printf("main thread - user: %ld.%06ld s, system: %ld.%06ld s\n",
           (long)main_usage.ru_utime.tv_sec, (long)main_usage.ru_utime.tv_usec,
           (long)main_usage.ru_stime.tv_sec, (long)main_usage.ru_stime.tv_usec);
    // GPT code end here
    // Free the memory
    free(threads);
    free(con);
    free(mutex);
    free(thread_datas);

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
