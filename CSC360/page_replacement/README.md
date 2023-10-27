# Overview
Upon getting a page fault, initially all the page table frames are free and therefore the pages are added without the need of a replacement algorithm. As the pages are being added, their corresponding frame number is added to the queue. In my implementation newer entries to the queue are added to the back of the queue to make access of the eldest entry easier as it will be at the head of the queue.

Once the page table no longer has any free frames, it is then that the page replacement algorithms are used in tandem.

## Data Structures Used
* Linked list in form of a queue

# Page Replacement Algorithms
# FIFO (First In First Out)
For this implementation the head of the queue corresponds to the eldest entry to the queue.

In the case that page being added already exists within the page table, therefore a page hit, the dirty bit of the page is set accordingly.

In the case that the page being added does not exist in the page table, therefore a page miss, first the page table entry corresponding to the frame at the front of the queue is updated to the new page details. After which the frame is rotated to the back of the queue with its new information because it is now the most recent entry to the queue. This is done by first adding it to the back of the queue then deleting the head of the queue.


# LRU (Least Recently Used)
For this implementation the head of the queue corresponds to the least recently used page frame.

In the case that page being added already exists within the page table, therefore a page hit, the dirty bit of the page is set accordingly. The corresponding frame number of the page is also removed from its position in the queue and placed at the back of the queue because it is now the most recently used.

In the case that the page being added does not exist in the page table, therefore a page miss, first the page table entry corresponding to the frame at the front of the queue is updated to the new page details. After which the frame is rotated to the back of the queue with its new information because it is now the most recently used frame. This is done by first adding it to the back of the queue then deleting the head of the queue.

# Second Chance
In this page algorithm, whenever a page is added to the page table its reference bit is first set to 1, whether or not there were any free frames.

In the case that page being added already exists within the page table, therefore a page hit, the dirty bit of the page is set accordingly. The reference bit of the page is also set to 1 and then the frame number corresponding to the page is removed from its position in the queue and placed at the back of the queue.

In the case that the page being added does not exist in the page table, therefore a page miss, first I iterate through the queue to find the first entry that has its reference bit set to 0. In the process of doing so, all of the entries that are passed with reference bit 1 get their reference bit set to 0.

Supposing the first entry that has its reference bit as 0 is the head of the queue, the FIFO algorithm is applied to the entire queue, adding the new page to the page table with its reference bit set to 1.

However, if that is not the case, upon finding the first entry that already had its reference bit as 0, a temporary queue is created that stores all the entries following and including said entry. The FIFO algorithm is the applied to this temporary queue adding the new page to the page table with its reference bit set to 1.

After all this the temporary queue is appended to the actual queue in the correct position it was created from.