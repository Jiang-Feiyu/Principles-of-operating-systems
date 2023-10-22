# Principles-of-operating-systems
This is the course material of COMP3230
# Set up computing platform
## Install the docker application and run a Ubuntu docker container on Windows or Mac (intel or Apple CPU)
- Download: 
</br> For Windows: <https://docs.docker.com/desktop/install/windows-install/>
</br> For Mac: <https://docs.docker.com/desktop/install/mac-install/>
</br> Remember to login the docker after download.
- Pull the image:
</br> `docker pull atctam/c3230-m1-ubuntu` or `docker pull atctam/c3230-ubuntu`
- Notice, for windows may need `wsl --update`
- Open a terminal at that local directory
- Run the docker: `docker container run -w "/home/c3230" -v "$(pwd):/home/c3230" -it atctam/c3230-m1-ubuntu bash` or `docker container run -w "/home/c3230" -v "$(pwd):/home/c3230" -it atctam/c3230-ubuntu bash`
- Stop the docker process:
</br> `docker ps`
</br> `docker stop $(CONTAINER ID)`
- When running docker, a new container will be created everytime. Can delete container by `docker rm $(container name / id)`
