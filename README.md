# system-monitor
A process monitor system that monitors all the active processes, with their corresponding process ids (PIDs), CPU usage, memory usage
## Background
### MS Process Minitor 
Process Monitor is an advanced monitoring tool for Windows that shows real-time file system, Registry and process/thread activity. It combines the features of two legacy Sysinternals utilities, Filemon and Regmon, and adds an extensive list of enhancements including rich and non-destructive filtering, comprehensive event properties such as session IDs and user names, reliable process information, full thread stacks with integrated symbol support for each operation, simultaneous logging to a file, and much more

* System Data
    * Linux stores a lot of system data in files within the `/proc` directory. 

* Operating System
    * Information about the operating system exists outside of the `/proc` directory, in the `/etc/os-release` file.
    * There are several strings from which to choose here, but the most obvious is the value specified by "PRETTY_NAME".

* Kernel
    * Information about the kernel exists `/proc/version` file.
* Memory Utilization
    * Information about memory utilization exists in the `/proc/meminfo` file.
    * There are a variety of ways to use this data to calculate memory utilization.
    * [Stack Overflow answer about how htop calculates memory utilization from the data in /proc/meminfo.]()

* Total Processes
    * Information about the total number of processes on the system exists in the `/proc/meminfo` file.
* Running Processes
    * Information about the number of processes on the system that are currently running exists in the `/proc/meminfo` file.
* Up Time
    * Information about system up time exists in the `/proc/uptime` file.

## Reference
[MS Process Monitor](https://docs.microsoft.com/en-us/sysinternals/downloads/procmon)
[Mac Activity Monitor](https://support.apple.com/guide/activity-monitor/welcome/mac)
[Linux Top](https://man7.org/linux/man-pages/man1/top.1.html)

