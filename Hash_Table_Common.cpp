#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//จินตนาการว่ามีตู้แนวตั้งยาวมากๆ(size arr)แล้วมีล็อกเกอร์เรียงยาวหลายชั้น(index)
#define TABLE_SIZE 10//ขนาดarr(ตู้แนวตั้งที่มีล็อกเกอร์10ล็อก)ที่เก็บข้อมูลvalueไว้(ใส่น้อยๆจะได้ชนกันง่าย)
//Link listเอาไว้จัดการตอนindexมันเท่ากันเพื่อที่จะเอาไปต่อท้าย
typedef struct Node {
    char key[50];
    float value;
    struct Node *next;
} Node;

typedef struct HashTable {
    Node *buckets[TABLE_SIZE];
} HashTable;
// Hash function แบบbasic โดยแปลงรหัสนักศึกษาเป็น index (เอาไว้โยนเข้าล็อกเกอร์)
unsigned int hash_function(const char *key) {
    unsigned int hash = 5381; //อัลกอริทึมของ djb2//Dan Bernstein (ผู้คิดค้น djb2)
    int c;
    while ((c = *key++)) //เอาค่าเดิมมาขยายขึ้น (คูณด้วย 32)
        hash = ((hash << 5)/*left shiftเลื่อนบิตไปทางซ้าย5ตำแหน่ง pow(2,5)*/ + hash) + c; // บวกค่าเดิมอีกที (รวมเป็นคูณด้วย 33) + ค่าASCIIของc
    return hash % TABLE_SIZE; //mod ด้วย size array
}
// สร้างตู้และเตรียมล็อกเกอร์10ชั้น
HashTable* create_table() {
    HashTable *t = (HashTable*)malloc(sizeof(HashTable));
    for (int i = 0; i < TABLE_SIZE; i++) t->buckets[i] = NULL;
    return t;
}
//ตรวจว่ามีสิ่งของอยู่ในล็อกเกอร์นี้ไหม
void upsert(HashTable *t, char *key, float value) {
    unsigned int index = hash_function(key); 
    Node *current = t->buckets[index]; //indexที่ได้เอาสิ่งของไว้ที่นี่

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) { //ถ้าเจอชื่อเดียวกัน
            current->value = value;//เปลี่ยนเกรดข้างในกล่องเดิม
            printf(">> Updated %s with GPA %.2f\n", key, value);
            return;
        }
        current = current->next;
    }
//ถ้าไม่เจอสร้างกล่องใหม่
    Node *new_node = (Node*)malloc(sizeof(Node));
    strcpy(new_node->key, key);
    new_node->value = value;
    new_node->next = t->buckets[index]; // เอาเชือกจากกล่องใหม่ไปผูกกับ "กล่องที่เคยอยู่อันดับแรก" ของล็อกนั้น
    t->buckets[index] = new_node; //เอากล่องใหม่ไว้หน้าสุด//จะได้(O(1))
    printf(">> Inserted %s with GPA %.2f (Bucket Index: %d)\n", key, value, index);
}
//function searchหาของในล็อกเกอร์
void search_key(HashTable *t, char *key) {
    unsigned int index = hash_function(key); //โยนเข้าHash function
    Node *current = t->buckets[index]; //ได้ค่าออกมาแล้วไปเปิดล็อกเกอร์เอาของออกมา
    //ค่อยๆหาของตามสายเชือกในล็อกเกอร์ชั้นนั้นๆ
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            printf(">> Found: Student ID %s has GPA %.2f\n", key, current->value);
            return;
        }
        current = current->next;
    }
    printf(">> Not Found: Student ID %s does not exist.\n", key);
}
//function ไว้ลบกล่องในล็อกเกอร์
void delete_key(HashTable *t, char *key) {
    unsigned int index = hash_function(key);
    Node *current = t->buckets[index];
    Node *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev == NULL) {
                // ถ้ากล่องที่ลบอยู่ใบแรกสุดของชั้น ให้กล่องถัดไปขึ้นมาอยู่หน้าสุดแทน
                t->buckets[index] = current->next;
            } else {
                // ถ้าอยู่ตรงกลาง ให้เอาเชือกจากกล่องก่อนหน้า ข้ามไปผูกกับกล่องถัดไปเลย
                prev->next = current->next;
            }
            free(current); //free memoryกล่องที่เอาออกไป
            printf(">> Deleted: Student ID %s\n", key);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf(">> Error: Cannot delete %s (Key not found)\n", key);//ตอนไม่พบกล่องที่ต้องการลบ
}
//แสดงผล//จะออกแบบ(Last-In, First-Out สำหรับล็อกนั้นๆจากการแทรกที่หัวแถว)
void display_all(HashTable *t) {
    printf("\n--- Current Hash Table State ---\n");//สภาพล็อกเกอร์
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *curr = t->buckets[i];
        if (curr != NULL) {
            printf("Bucket [%d]: ", i);//ชั้น
            while (curr != NULL) {
                printf("[%s: %.2f] -> ", curr->key, curr->value);
                curr = curr->next;
            }
            printf("NULL\n");//ว่าง
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
