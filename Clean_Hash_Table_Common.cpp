#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct Node {
    char key[50];
    float value;
    struct Node *next;
} Node;

typedef struct HashTable {
    Node *buckets[TABLE_SIZE];
} HashTable;

unsigned int hash_function(const char *key) {
    unsigned int hash = 5381;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c;
    return hash % TABLE_SIZE;
}

HashTable* create_table() {
    HashTable *t = (HashTable*)malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) t->buckets[i] = NULL;
    return t;
}

void upsert(HashTable *t, char *key, float value) {
    unsigned int index = hash_function(key); 
    Node *current = t->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            printf(">> Updated %s with GPA %.2f\n", key, value);
            return;
        }
        current = current->next;
    }

    Node *new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->key, key);
    new_node->value = value;
    new_node->next = t->buckets[index];
    t->buckets[index] = new_node;
    printf(">> Inserted %s with GPA %.2f (Bucket Index: %d)\n", key, value, index);
}

void search_key(HashTable *t, char *key) {
    unsigned int index = hash_function(key);
    Node *current = t->buckets[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            printf(">> Found: Student ID %s has GPA %.2f\n", key, current->value);
            return;
        }
        current = current->next;
    }
    printf(">> Not Found: Student ID %s does not exist.\n", key);
}

void delete_key(HashTable *t, char *key) {
    unsigned int index = hash_function(key);
    Node *current = t->buckets[index];
    Node *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                t->buckets[index] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            printf(">> Deleted: Student ID %s\n", key);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf(">> Error: Cannot delete %s (Key not found)\n", key);
}

void display_all(HashTable *t) {
    printf("\n--- Current Hash Table State ---\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *curr = t->buckets[i];
        if (curr != NULL) {
            printf("Bucket [%d]: ", i);
            while (curr != NULL) {
                printf("[%s: %.2f] -> ", curr->key, curr->value);
                curr = curr->next;
            }
            printf("NULL\n");
        }
    }
    printf("--------------------------------\n");
}

int main() {
    HashTable *ht = create_table();
    int choice;
    char key[50];
    float val;

    while(1) {
        printf("\n=== Student Grade System (Hash Map) ===\n");
        printf("1. Insert/Update Grade\n");
        printf("2. Search Grade\n");
        printf("3. Delete Student\n");
        printf("4. Show All Data\n");
        printf("5. End program\n");
        printf("Select option: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                printf("Enter Student ID: ");
                scanf("%s", key);
                printf("Enter GPA: ");
                scanf("%f", &val);
                upsert(ht, key, val);
                break;
            case 2:
                printf("Enter Student ID to Search: ");
                scanf("%s", key);
                search_key(ht, key);
                break;
            case 3:
                printf("Enter Student ID to Delete: ");
                scanf("%s", key);
                delete_key(ht, key);
                break;
            case 4:
                display_all(ht);
                break;
            case 5:
                printf("End program.....\n");
                return 0;
            default:
                printf("Invalid option. Please try again.\n");
        }
    }
    return 0;
}
