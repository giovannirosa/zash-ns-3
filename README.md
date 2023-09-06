# ZASH (Zero-Aware Smart Home System)

Although smart homes have become popular recently, people are still highly concerned about security, safety, and privacy issues. Studies revealed that issues in people’s privacy generate physiological and financial harm because smart homes are intimate living environments. Further, our research disclosed that impersonation attacks are one of the most severe threats against smart homes because they compromise confidentiality, authenticity, integrity, and non-repudiation. Typically, approaches to build security for Smart Home Systems (SHS) require historical data to implement access control and Intrusion Detection Systems (IDS), a vulnerability to the inhabitant’s privacy. Additionally, most works rely on cloud computing or resources in the cloud to perform security tasks, which attackers can exploit to target confidentiality, integrity, and availability. Moreover, researchers do not regard the misuse of SHS by forcing users to interact with devices through their smartphones or tablets, as they usually interact by any means, like virtual assistants and devices themselves. Therefore, the security system requirements for smart homes should comprehend privacy perception, low latency in response, spatial and temporal locality, device extensibility, protection against impersonation, device isolation, access control enforcement, and taking into account the refresh verification with a trustworthy system. To attend to those requirements, we propose the ZASH (Zero-Aware Smart Home) system to provide access control for the user’s actions on smart devices in smart homes. In contrast to current works, it leverages continuous authentication with the Zero Trust paradigm supported by configured ontologies, real-time context, and user activity. Edge computing and Markov Chain enable ZASH to prevent and mitigate impersonation attacks that aim to compromise users’ security. The system relies only on resources inside the house, is self-sufficient, and is less exposed to outside exploitation. Furthermore, it works from day zero without the requirement of historical data, though it counts on that as time passes to monitor the users’ behavior. ZASH requires proof of identity for users to confirm their authenticity through strong features of the Something You Are class. The system enforces access control in smart devices, so it does not depend on intermediaries and considers any user-device interaction. At first, an initial test of algorithms with a synthetic dataset demonstrated the system’s capability to dynamically adapt to new users’ behaviors withal blocking impersonation attacks. Finally, we implemented ZASH in the ns-3 network simulator and analyzed its robustness, efficiency, extensibility, and performance. According to our analysis, it protects users’ privacy, responds quickly (around 4.16 ms), copes with adding and removing devices, blocks most impersonation attacks (up to 99% with a proper configuration), isolates smart devices, and enforces access control for all interactions.

# Execution

The requirement to run the simulation is [ns-3 3.36.1](https://www.nsnam.org/releases/ns-3-36/). Follow the instructions to install and configure ns-3 on the official website.

There are two scripts to help execute the code:
1. replace.sh: copy necessary files from this repository to the ns-3 directory.
2. run.sh: execute the system with the necessary parameters

While and after executing the simulation, logs are generated under zash_traces/ in the date and time it started.

It's possible to visualize data using the chart's R scripts. For that, I recommend using [R Studio](https://posit.co/download/rstudio-desktop/).

# Paper

For more information and details about this work, please refer to the [dissertation](./_ME_Giovanni__Dissertação_final.pdf).

