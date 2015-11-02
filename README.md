SavingsAccount
==========

An exercise in learning Linux System V Semaphores, and other commands for creating concurrently executing Linux processes.  The program imitates a savings account with customers looking to withdraw and deposit money.  Withdrawers are all served FIFO, and the head of the line blocks the rest of the queue until there is enough money in the account to service it.
