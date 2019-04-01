/*
给出两个 非空 的链表用来表示两个非负的整数。其中，它们各自的位数是按照 逆序 的方式存储的，并且它们的每个节点只能存储 一位 数字。

如果，我们将这两个数相加起来，则会返回一个新的链表来表示它们的和。

您可以假设除了数字 0 之外，这两个数都不会以 0 开头。

示例：

输入：(2 -> 4 -> 3) + (5 -> 6 -> 4)
输出：7 -> 0 -> 8
原因：342 + 465 = 807


*/

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode *node= (struct ListNode*)malloc(sizeof(struct ListNode));
    struct ListNode *head = node;
    int val1=0,val2=0,count=0;
    while(l1!=NULL&&l2!=NULL&&count!=0)
    {
        if(node!=head)
        {
            node->next = (struct ListNode*)malloc(sizeof(struct ListNode));
            node = node->next;
        }
	
        val1 = (l1 == NULL)? 0 : l1->val;
        val2 = (l2 == NULL)? 0 : l2->val;
        count = (val1 + val2) % 10;
        node->val = (val1 + val2) / 10 + count;
    }
    return head;
}




