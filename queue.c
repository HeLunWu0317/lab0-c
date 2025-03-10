#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
// NOTE: OK
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        // if correct, set the linked
        // head->next = head;
        // head->prev = head;
        // just use INIT_LIST_HEAD
        INIT_LIST_HEAD(head);

        return head;
    }
    return NULL;
}

/* Free all storage used by queue */
// NOTE: OK

void q_free(struct list_head *head)
{
    if (!head)
        return;
    else {
        struct list_head *now;
        struct list_head *next;
        list_for_each_safe(
            now, next,
            head) {  // use this function to find every node in this queue
            // get the element for the now node
            element_t *get = list_entry(now, element_t, list);
            // release the element from now node

            if (get->value) {
                free(get->value);
            }
            list_del(now);
            free(get);
        }
        // free the memory from head
        free(head);
    }
}

/* Insert an element at head of queue */
// NOTE: OK
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_data =
        malloc(sizeof(element_t));  // allocate new memory for the new data
    // maybe malloc fail, so need to check
    if (!new_data)
        return false;
    // new_data->value = strdup(s);  // use strdup to copy string value to
    // new_data
    size_t length = strlen(s) + 1;
    new_data->value = (char *) malloc(length * sizeof(char));
    if (!new_data->value) {
        free(new_data);
        return false;
    }
    snprintf(new_data->value, length, "%s", s);
    list_add(&new_data->list, head);
    return true;
}


/* Insert an element at tail of queue */
// NOTE: OK, T.C not good
//
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }
    element_t *new_data =
        malloc(sizeof(element_t));  // allocate new memory for the new data
    if (!new_data) {
        return false;
    }
    // copy value
    // new_data->value = strdup(s);
    size_t length = strlen(s) + 1;
    new_data->value = (char *) malloc(length * sizeof(char));
    if (!new_data->value) {
        free(new_data);
        return false;
    }
    snprintf(new_data->value, length, "%s", s);

    list_add_tail(&new_data->list, head);
    return true;
}


/* Remove an element from head of queue */
// FIXME: my code didn't use the bufsize,need to check
// NOTE: FIXME is solved
// NOTE: OK, T.C not good

element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // check the head
    if (!head || list_empty(head)) {  // if the the queue is empty
        return NULL;                  // return NULL, as the header request
    }

    // remove the target node to the other position
    struct list_head *tmp =
        head->next;  // get the head->next and save into the tmp node
    element_t *get_data =
        list_entry(tmp, element_t, list);  // get the the data of head->next
    list_del(tmp);                         // delete node first
    if (sp) {
        // strncpy(sp, get_data->value, bufsize);  // copy the get_data->value
        // to
        strncpy(sp, get_data->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return get_data;  // return the pointer to the data
}

/* Remove an element from tail of queue */
// TODO: Maybe some bug need to fix
// NOTE: OK

element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    // get the element with node
    struct list_head *tmp = head->prev;
    element_t *get_data = list_entry(tmp, element_t, list);
    // delete the tmp node first
    list_del(tmp);
    if (sp && get_data->value) {
        strncpy(sp, get_data->value,
                bufsize);  // copy the get_data->value to the sp
        // sp[bufsize - 1] = '\0';  // set the last character is \0
    }
    return get_data;  // return the pointer to the data
}

/* Return number of elements in queue */
// NOTE: OK
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each(li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
// NOTE: OK

bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    else {
        int check = 0;
        struct list_head *now;
        list_for_each(
            now,
            head) {  // use this function to find every node in this queue
            check++;
        }
        int n = (check - 1) / 2;
        // now = head->next;  // find the value from head
        struct list_head *next;
        int index = 0;
        list_for_each_safe(
            now, next,
            head) {  // use this function to find every node in this queue
            if (index == n) {
                list_del(now);
                // we should free value first, and then free the element_t
                element_t *get_data = list_entry(now, element_t, list);
                free(get_data->value);
                free(get_data);
                return true;
            }
            index++;
        }
    }
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return false;
}

/* Delete all nodes that have duplicate string */
// FIXME: still Segmentation fault occurred.  You dereferenced a NULL or invalid
// pointer---	trace-06-ops	0/6
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    bool check_del = false;  // set a boolean to check dup
    struct list_head *now;   // to find every node
    struct list_head *next;

    list_for_each_safe(now, next, head) {
        element_t *now_elem = list_entry(now, element_t, list);  // get data
        bool dup_check = false;
        // check value
        // NOTE: use if to check, while is too inenficient
        if (next == head) {
            break;
        }
        element_t *next_elem = list_entry(next, element_t, list);
        if (strcmp(now_elem->value, next_elem->value) == 0) {  // check dup
            // find it
            dup_check = true;
            // release the memory
            list_del(next);
            free(next_elem->value);
            free(next_elem);
            // goto next
            next = now->next;
        }
        if (dup_check) {  // do the delete
            list_del(now);
            free(now_elem->value);
            free(now_elem);
            check_del = true;
        }
    }

    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return check_del;
}
// bool q_delete_dup(struct list_head *head)
// {
//     if (!head || list_empty(head)) {
//         return false;
//     }

//     bool check_del = false;  // set a boolean to check dup
//     element_t *now;          // to find every node
//     element_t *next;
//     // first to list
//     list_for_each_entry_safe(now, next, head, list) {
//         bool dup_check = false;
//         // check value
//         element_t *group_head;
//         element_t *group_next;
//         // second list to  find dup
//         list_for_each_entry_safe(group_head, group_next, head, list) {
//             if (group_head == now) {
//                 continue;
//             }
//             if (strcmp(group_head->value, now->value) == 0) {
//                 dup_check = true;
//                 list_del(&group_head->list);
//                 free(group_head->value);
//                 free(group_head);
//             } else {
//                 continue;
//             }
//         }
//         if (dup_check) {  // delete now
//             list_del(&now->list);
//             free(now->value);
//             free(now);
//             check_del = true;
//         }
//     }

//     // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
//     return check_del;
// }

/* Swap every two adjacent nodes */
// FIXME: swap fail
// NOTE: OK
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *now = head->next;  // set to head next, head not a node
    struct list_head *safe;              // set safe to save next
    while (now != head && now->next != head) {
        safe = now->next->next;           // save the next head
        list_move(now->next, now->prev);  // move second one to first's prev
        now = safe;                       // go to next
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
// use list_move
// NOTE: OK
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    // set some variable ,and start from head
    struct list_head *now = head->next;
    struct list_head *tmp_now;
    while (now != head) {
        tmp_now = now->next;
        list_move(now, head);
        now = tmp_now;
    }
}

/* Reverse the nodes of the list k at a time */
// use list_cut_position & list_splice, cut the k_size list and call q_reverse,
// and do the list_splice
// NOTE:OK
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 2) {  // need to check k_size
        return;
    }

    // INIT
    int count = 0;                       // use count check k
    struct list_head *now = head->next;  // saved the node now
    struct list_head *tmp;               // saved the head now
    struct list_head temp_head;          // saved the reverse list
    INIT_LIST_HEAD(&temp_head);
    // do the reverse for size_k
    while (now != head) {
        count++;
        if (count == k) {
            tmp = now->next;  // save the next head
            list_cut_position(&temp_head, head,
                              now);  // got the k_size group
            q_reverse(&temp_head);   // do the reverse for the group
            list_splice(&temp_head, head);
            now = tmp;
            count = 0;
        } else {
            now = now->next;
        }
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}


// /* Sort elements of queue in ascending/descending order */
// // FIXME: too inefficient
// NOTE: Merge sort is fail
// void q_sort(struct list_head *head, bool descend)
// {
//     // check init
//     if (!head || list_empty(head) || list_is_singular(head)) {
//         return;
//     }

//     // use fast-slow pointer find the middle,prepare to splite
//     struct list_head *fast = head->next;
//     struct list_head *slow = head->next;
//     while (fast != head && fast->next != head) {    //need to check
//     next->next too
//         slow = slow->next;        // jump one step per times
//         fast = fast->next->next;  // jump two step per times
//     }
//     struct list_head *right = slow->next;
//     // get left(head) & right
//     slow->next = head;        // set the next to head
//     head->prev = slow;        // update head->prev
//     // recursive q_sort
//     q_sort(head, descend);
//     q_sort(right, descend);
//     // merge and complete mergesort
//     struct list_head New_Queue;  // create new queue to save sorted list
//     INIT_LIST_HEAD(&New_Queue);

//     while (!list_empty(right) && !list_empty(head)) {
//         element_t *q1 = list_first_entry(head, element_t, list);
//         element_t *q2 = list_first_entry(right, element_t, list);
//         //need to check descend or ascend
//         if ((descend && strcmp(q1->value, q2->value) > 0) || (!descend &&
//         strcmp(q1->value, q2->value) < 0)) {
//             list_move_tail(head->next, &New_Queue);  // q1 value is small
//         } else {
//             list_move_tail(right->next, &New_Queue);  // q2 value is small
//         }
//     }

//     // New_Queue linking to head, finish
//     if (!list_empty(head)) {
//         list_splice_tail(head, &New_Queue);
//     }
//     if (!list_empty(right)) {
//         list_splice_tail(right, &New_Queue);
//     }
//     list_splice(&New_Queue, head);
// }
// use insertion sort O(n^2), not efficient
void q_sort(struct list_head *head, bool descend)
{
    // check init
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    // start from second one
    struct list_head *now = head->next->next;
    while (now != head) {
        // save next to temp the now->next
        struct list_head *next = now->next;

        // del the now
        list_del(now);

        // find the insertion target
        struct list_head *target = now->prev;
        while (target != head) {
            const element_t *now_entry = list_entry(now, element_t, list);
            const element_t *target_entry = list_entry(target, element_t, list);

            // start compare
            if ((descend &&
                 strcmp(now_entry->value, target_entry->value) <= 0) ||
                (!descend &&
                 strcmp(now_entry->value, target_entry->value) >= 0)) {
                break;
            }
            target = target->prev;
        }

        // add to the tail
        list_add(now, target);

        // go to next
        now = next;
    }
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
// FIXME: some bug
int q_ascend(struct list_head *head)
{
    // check init
    if (!head || list_empty(head) || list_is_singular(head)) {
        return q_size(head);
    }
    // use while loop to check, start from head
    struct list_head *now = head->prev;
    while (now != head && now->prev != head) {
        // get max
        const element_t *max = list_entry(now, element_t, list);
        // get prev value
        element_t *check_value = list_entry(now->prev, element_t, list);
        // start to compare
        if (strcmp(check_value->value, max->value) < 0) {  // check_value < max
            now = now->prev;                               // goto prev
            continue;
        } else {
            struct list_head *tmp = now->prev;
            list_del(tmp);
            free(check_value->value);
            free(check_value);
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}


/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
// FIXME: still fail
int q_descend(struct list_head *head)
{
    // check init
    if (!head || list_empty(head) || list_is_singular(head)) {
        return q_size(head);
    }
    // use while loop to check, start from tail
    struct list_head *now = head->prev;
    while (now != head && now->prev != head) {
        // get min
        const element_t *min = list_entry(now, element_t, list);
        // get prev value
        element_t *check_value = list_entry(now->prev, element_t, list);
        // start to compare
        if (strcmp(check_value->value, min->value) < 0) {  // check_value < min
            struct list_head *tmp = now->prev;
            list_del(tmp);
            free(check_value->value);
            free(check_value);
        } else {
            now = now->prev;  // goto prev
            continue;
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
// need to use q_context:
/**
 * queue_contex_t - The context managing a chain of queues
 * @q: pointer to the head of the queue
 * @chain: used by chaining the heads of queues
 * @size: the length of this queue
 * @id: the unique identification number
typedef struct {
    struct list_head *q;
    struct list_head chain;
    int size;
    int id;
} queue_contex_t;
  */
int q_merge(struct list_head *head, bool descend)
{
    /// check init
    if (!head || list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        queue_contex_t *final = list_entry(head->next, queue_contex_t, chain);
        return q_size(final->q);
    }
    // merge other queue with first
    struct list_head *first_queue =
        list_entry(head->next, queue_contex_t, chain)->q;

    struct list_head *now;
    struct list_head *next;
    list_for_each_safe(now, next, head) {
        queue_contex_t *now_queue = list_entry(now, queue_contex_t, chain);

        // start with second queue
        if (now_queue->q == first_queue) {
            continue;
        }
        // save the subqueue
        struct list_head *subqueue = now_queue->q;

        // create new queue to save merged queue
        struct list_head final;
        INIT_LIST_HEAD(&final);
        // start to sort
        while (!list_empty(first_queue) && !list_empty(subqueue)) {
            const element_t *elem_1 =
                list_first_entry(first_queue, element_t, list);
            const element_t *elem_2 =
                list_first_entry(subqueue, element_t, list);

            if ((descend && strcmp(elem_1->value, elem_2->value) > 0) ||
                (!descend &&
                 strcmp(elem_1->value, elem_2->value) < 0)) {  // elem_2 bigger
                list_move_tail(first_queue->next,
                               &final);  // go to new queue tail
            } else {
                list_move_tail(subqueue->next, &final);  // go to new queue tail
            }
        }
        // remind elem go to final
        if (!list_empty(first_queue)) {
            list_splice_tail_init(first_queue, &final);
        }
        if (!list_empty(subqueue)) {
            list_splice_tail_init(subqueue, &final);
        }
        list_splice(&final, first_queue);
        // free the subqueue
        INIT_LIST_HEAD(subqueue);
    }

    // https://leetcode.com/problems/merge-k-sorted-lists/
    return q_size(first_queue);
}


// can't do it
//  int q_merge(struct list_head *head, bool descend)
//  {
//      /// check init
//      if (!head || list_empty(head)) {
//          return 0;
//      }
//      if(list_is_singular(head)){
//          return q_size(head);
//      }
//      struct list_head *first = head->next;

//     struct list_head *now;
//     struct list_head *tmp;

//     list_for_each_safe(now, tmp, head){
//         if(now == first){
//             continue;
//         }
//         struct list_head *subqueue = now;
//         list_splice_tail_init(subqueue, first);
//     }
//     q_sort(first, descend);

//     // https://leetcode.com/problems/merge-k-sorted-lists/
//     return q_size(first);
// }
