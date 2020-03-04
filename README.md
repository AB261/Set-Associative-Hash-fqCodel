# Validation of Set Associative Hash used in FqCodel in ns-3
The data for the graph is generated through the use of files in this repository. 
## Steps to replicate the graph
* Python code generates the random hash values which will be attributed to each packet
* FqCoDelQueueDiscCollision TestSuite takes as input the file containing the hash values and inputs it into the modified fq-codel-queue-disc 
* The boolean variable SetAssociativeHash can be used to enable/disable the Set Associativity of FqCodel
* The test will output collision probability(sets filled/ total sets or queues filled/total queues) for either the set-associative hash or traditional hash depending on which is enabled in the test suite 
* The probabilities are output into the console
* Input the probabilities into excel sheet and graph probability vs. the number of flows graph 
* Perform more experiments to get a smoother curve
* Our hash value files are included along with the excel sheet and the generated graph

### Testing:

Commands to Run:
```shell
NS_LOG="FqCoDelQueueDisc" ./test.py -s fq-codel-queue-disc --text=results

NS_LOG="FqCoDelQueueDisc" ./waf --run "test-runner --suite=fq-codel-queue-disc"

NS_LOG="FqCoDelQueueDisc:FqCoDelQueueDisc2:PacketFilter:QueueDisc" ./waf --run "test-runner --suite=fq-codel-queue-disc"

NS_LOG="FqCoDelQueueDisc" ./waf --run "test-runner --suite=fq-codel-queue-disc" > <generated-file-name>
```