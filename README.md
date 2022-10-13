# MapReduce-Model-Using-Pipes
In this project, a word counter is implemented using the MapReduce architecture in order to leran how to manage the processes and communication ways between them.

In this system architecture, three types of processes are considered with the names of `Main` process, `Map` process and `Reduce` process:
* **Main Process**: This process is considered the parent process of the system and its task is to create `Reduce` and `Map` processes. This process creates a child process of the `Map` type according to the number of csv files in the `testcases` directory and specify their file names. `unnamed` pipe is used to transfer the file name to the `Map` process. Also, a `Reduce` process is created in this process, which sends the final output to the `Main` process. At this stage, the `Main` process should record the results in the `output.csv` file.
* **Map Process**: Each of the `Map` processes has received the name of the file on which they must perform operations. At this stage, each process calculates the number of word occurrences in the corresponding file. The output of this section is a number of keys (words) and their corresponding values (number of word repetitions). Each `Map` process sends its output to the `Reduce` process using a `named` pipe.
* **Reduce Process**: This process, created by the `Main` process, waits to receive the output of all `Map` processes. In this step, the outputs are combined and the number of occurrences of each word among all files is calculated. Finally, this output is sent to the `Main` process using an `unnamed` pipe.

To create processes by the `Main` process, `fork` and `exec` system calls have been used to create and execute them.






