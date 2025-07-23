## Computer Systems (COMP30023)

### Course Description
[Computer Systems, University of Melbourne Handbook](https://handbook.unimelb.edu.au/2025/subjects/comp30023)

Over the last half-century, computers have improved at a faster rate than almost any other technology on the planet, yet the principles on which they work have remained mostly constant. In this subject, students will learn how computer systems work "under the hood".

The specific aim of this subject is for the students to develop an understanding of the basic concepts underlying computer systems. A key focus of this subject is the introduction of operating systems principles and computer network protocols. This knowledge is essential for writing secure software, for writing high performance software, and for writing network-based services and applications.

### Assignments Overview

- **Assignment 1: Virtual Memory Management**
  - In this project, I implemented a virtual memory manager in C that simulates the translation of logical addresses to physical addresses for a single process. My program, translate, parses memory access traces and uses a page table and a TLB to manage address translation. I implemented FIFO page replacement for handling page faults when memory is full and integrated a TLB with an LRU replacement policy. The program supports different simulation modes (Tasks 1–4), and I ensured it strictly follows the specified output format for automated testing. I also maintained clean build practices with a Makefile and applied good coding standards, version control, and memory management throughout development.

- **Assignment 2: Web Proxy**
  - In this collaborative project for COMP30023, I worked with a partner to develop a caching web proxy for HTTP/1.1 in C, focusing on socket programming and network protocols. Our proxy handled GET requests by forwarding them to the appropriate server and returning the response to the client, with support for both IPv4 and IPv6. We implemented features in stages, starting with basic request forwarding, then adding naive and valid caching mechanisms, respecting HTTP Cache-Control directives, and enforcing cache expiration based on max-age. We also aimed for good software practices, safe memory management, and continuous integration using GitHub, ensuring our code compiled and ran reliably on the provided cloud VM.

### Technologies Used
- **C** for low-level algorithm implementation to learn the algorithms.


