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
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head) {
        INIT_LIST_HEAD(head);
        return head;
    }
    return NULL;
}

/* Free all storage used by queue */

void q_free(struct list_head *head)
{
    if (!head)
        return;
    else {
        struct list_head *now;
        struct list_head *next;
        list_for_each_safe(now, next, head) {
            element_t *get = list_entry(now, element_t, list);
            if (get->value) {
                free(get->value);
            }
            list_del(now);
            free(get);
        }
        free(head);
    }
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)
        return false;
    element_t *new_data = malloc(sizeof(element_t));
    if (!new_data)
        return false;
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
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }
    element_t *new_data = malloc(sizeof(element_t));
    if (!new_data) {
        return false;
    }
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

element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // check the head
    if (!head || list_empty(head)) {
        return NULL;
    }

    struct list_head *tmp = head->next;
    element_t *get_data = list_entry(tmp, element_t, list);
    list_del(tmp);
    if (sp) {
        strncpy(sp, get_data->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return get_data;
}

/* Remove an element from tail of queue */

element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    struct list_head *tmp = head->prev;
    element_t *get_data = list_entry(tmp, element_t, list);
    list_del(tmp);
    if (sp && get_data->value) {
        strncpy(sp, get_data->value, bufsize);
    }
    return get_data;
}

/* Return number of elements in queue */
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

bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    else {
        int check = 0;
        struct list_head *now;
        list_for_each(now, head) {
            check++;
        }
        int n = (check - 1) / 2;
        struct list_head *next;
        int index = 0;
        list_for_each_safe(now, next, head) {
            if (index == n) {
                list_del(now);
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

bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    bool final_check = false;
    struct list_head *now, *check;

    list_for_each(now, head) {
        element_t *now_data = list_entry(now, element_t, list);
        bool dup_check = false;

        list_for_each(check, head) {
            if (now == check)
                continue;
            element_t *check_data = list_entry(check, element_t, list);
            if (strcmp(now_data->value, check_data->value) == 0) {
                free(check_data->value);
                check_data->value = strdup("NeedToDel");
                dup_check = true;
            }
        }
        if (dup_check) {
            free(now_data->value);
            now_data->value = strdup("NeedToDel");
            final_check = true;
        }
    }
    struct list_head *next_check;
    struct list_head *now_check;
    // delete marking
    list_for_each_safe(now_check, next_check, head) {
        element_t *now_check_data = list_entry(now_check, element_t, list);
        if (strcmp(now_check_data->value, "NeedToDel") == 0) {
            list_del(now_check);
            q_release_element(now_check_data);
        }
    }

    return final_check;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *now = head->next;
    struct list_head *safe;
    while (now != head && now->next != head) {
        safe = now->next->next;
        list_move(now->next, now->prev);
        now = safe;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */

void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }
    struct list_head *now = head->next;
    struct list_head *tmp_now;
    while (now != head) {
        tmp_now = now->next;
        list_move(now, head);
        now = tmp_now;
    }
}

/* Reverse the nodes of the list k at a time */

void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 2) {
        return;
    }

    struct list_head *now = head;
    struct list_head *next_group;
    struct list_head temp_head;
    INIT_LIST_HEAD(&temp_head);
    while (!list_empty(head)) {
        int count = 0;

        next_group = now->next;
        while (count < k && next_group != head) {
            count++;
            next_group = next_group->next;
        }
        if (count < k) {
            break;
        }

        INIT_LIST_HEAD(&temp_head);
        list_cut_position(&temp_head, now, next_group->prev);
        q_reverse(&temp_head);

        list_splice_init(&temp_head, now);

        now = next_group->prev;
    }

    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}


/* Sort elements of queue in ascending/descending order */

static struct list_head *q_merging(struct list_head *head1,
                                   struct list_head *head2,
                                   bool descend)
{
    struct list_head *head = NULL;
    struct list_head **tail = &head;

    while (head1 && head2) {
        const char *str1 = list_entry(head1, element_t, list)->value;
        const char *str2 = list_entry(head2, element_t, list)->value;

        if (((strcmp(str1, str2) <= 0) && !descend) ||
            ((strcmp(str1, str2) >= 0) && descend)) {
            *tail = head1;
            tail = &head1->next;
            head1 = head1->next;
        } else {
            *tail = head2;
            tail = &head2->next;
            head2 = head2->next;
        }
    }

    *tail = head1 ? head1 : head2;
    return head;
}

static void q_merge_final(struct list_head *head,
                          struct list_head *a,
                          struct list_head *b,
                          bool descend)
{
    struct list_head *tail = head;

    while (a && b) {
        const char *str1 = list_entry(a, element_t, list)->value;
        const char *str2 = list_entry(b, element_t, list)->value;

        if (((strcmp(str1, str2) <= 0) && !descend) ||
            ((strcmp(str1, str2) >= 0) && descend)) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
        }
    }

    struct list_head *remainder = a ? a : b;
    if (remainder) {
        tail->next = remainder;
        do {
            remainder->prev = tail;
            tail = remainder;
            remainder = remainder->next;
        } while (remainder);
    }

    tail->next = head;
    head->prev = tail;
}


void q_sort(struct list_head *head, bool descend)
{
    struct list_head *list = head->next;
    struct list_head *pending = NULL;
    size_t count = 0;

    if (!head || list == head->prev) {
        return;
    }

    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        for (bits = count; bits & 1; bits >>= 1) {
            tail = &(*tail)->prev;
        }

        if (bits) {
            struct list_head *a = *tail;
            struct list_head *b = a->prev;
            a = q_merging(b, a, descend);
            a->prev = b->prev;
            *tail = a;
        }
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    list = pending;
    pending = pending->prev;
    while (pending) {
        struct list_head *next = pending->prev;
        list = q_merging(pending, list, descend);
        pending = next;
    }
    q_merge_final(head, NULL, list, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */

int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return q_size(head);
    }
    struct list_head *now = head->prev;
    while (now != head && now->prev != head) {
        const element_t *max = list_entry(now, element_t, list);
        element_t *check_value = list_entry(now->prev, element_t, list);
        if (strcmp(check_value->value, max->value) < 0) {
            now = now->prev;
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
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return q_size(head);
    }
    struct list_head *now = head->prev;
    while (now != head && now->prev != head) {
        const element_t *min = list_entry(now, element_t, list);
        element_t *check_value = list_entry(now->prev, element_t, list);
        if (strcmp(check_value->value, min->value) < 0) {
            struct list_head *tmp = now->prev;
            list_del(tmp);
            free(check_value->value);
            free(check_value);
        } else {
            now = now->prev;
            continue;
        }
    }
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */

int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    if (list_is_singular(head)) {
        queue_contex_t *final = list_entry(head->next, queue_contex_t, chain);
        return q_size(final->q);
    }
    struct list_head *first_queue =
        list_entry(head->next, queue_contex_t, chain)->q;

    struct list_head *now;
    struct list_head *next;
    list_for_each_safe(now, next, head) {
        queue_contex_t *now_queue = list_entry(now, queue_contex_t, chain);

        if (now_queue->q == first_queue) {
            continue;
        }
        struct list_head *subqueue = now_queue->q;

        struct list_head final;
        INIT_LIST_HEAD(&final);
        while (!list_empty(first_queue) && !list_empty(subqueue)) {
            const element_t *elem_1 =
                list_first_entry(first_queue, element_t, list);
            const element_t *elem_2 =
                list_first_entry(subqueue, element_t, list);

            if ((descend && strcmp(elem_1->value, elem_2->value) > 0) ||
                (!descend && strcmp(elem_1->value, elem_2->value) < 0)) {
                list_move_tail(first_queue->next, &final);
            } else {
                list_move_tail(subqueue->next, &final);
            }
        }
        if (!list_empty(first_queue)) {
            list_splice_tail_init(first_queue, &final);
        }
        if (!list_empty(subqueue)) {
            list_splice_tail_init(subqueue, &final);
        }
        list_splice(&final, first_queue);
        INIT_LIST_HEAD(subqueue);
    }

    // https://leetcode.com/problems/merge-k-sorted-lists/
    return q_size(first_queue);
}
element_t *get_element_at(struct list_head *head, int index)
{
    struct list_head *node;
    int i = 0;
    list_for_each(node, head) {
        if (i == index)
            return list_entry(node, element_t, list);
        i++;
    }
    return NULL;
}
void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    //
    int len = q_size(head);
    for (int i = len - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        element_t *ei = get_element_at(head, i);
        element_t *ej = get_element_at(head, j);

        if (ei && ej) {
            char *tmp = ei->value;
            ei->value = ej->value;
            ej->value = tmp;
        }
    }

    return;
}
