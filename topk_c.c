// Compile with: gcc -o topk_c topk_c.c -O3 -lm (or clang)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> // For memcpy

#define N 1000
#define K 10

// --- Helper Functions ---
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Comparison function for qsort (descending order)
int compare_desc(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

// Comparison function for qsort (ascending order - used for final sort of K items)
int compare_asc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// --- Method 1: Min-Heap ---
// Basic Min-Heap implementation for top K
void heapify_down(int heap[], int heap_size, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap_size && heap[left] < heap[smallest])
        smallest = left;
    if (right < heap_size && heap[right] < heap[smallest])
        smallest = right;

    if (smallest != index) {
        swap(&heap[index], &heap[smallest]);
        heapify_down(heap, heap_size, smallest);
    }
}

void build_min_heap(int heap[], int heap_size) {
    for (int i = heap_size / 2 - 1; i >= 0; i--) {
        heapify_down(heap, heap_size, i);
    }
}

void heap_replace(int heap[], int heap_size, int new_val) {
     heap[0] = new_val;
     heapify_down(heap, heap_size, 0);
}

void find_top_k_heap(int numbers[], int n, int k, int result[]) {
    int* min_heap = (int*)malloc(k * sizeof(int));
    if (!min_heap) { perror("malloc heap failed"); exit(1); }

    // Initialize heap with first K elements
    memcpy(min_heap, numbers, k * sizeof(int));
    build_min_heap(min_heap, k);

    // Process remaining elements
    for (int i = k; i < n; i++) {
        if (numbers[i] > min_heap[0]) { // If number is larger than smallest in heap
            heap_replace(min_heap, k, numbers[i]);
        }
    }

    // Copy heap elements to result (they are the largest k, but not sorted)
    memcpy(result, min_heap, k * sizeof(int));
    free(min_heap);

    // Sort the final K results (optional, but good for comparison)
    qsort(result, k, sizeof(int), compare_desc);
}


// --- Method 2: Sorting ---
void find_top_k_sort(int numbers[], int n, int k, int result[]) {
    // Create a copy to sort
    int* sorted_numbers = (int*)malloc(n * sizeof(int));
     if (!sorted_numbers) { perror("malloc sort failed"); exit(1); }
    memcpy(sorted_numbers, numbers, n * sizeof(int));

    // Sort the entire array
    qsort(sorted_numbers, n, sizeof(int), compare_desc);

    // Copy the top K elements
    memcpy(result, sorted_numbers, k * sizeof(int));
    free(sorted_numbers);
}

// --- Method 3: Quickselect ---
// Partition function (Lomuto partition scheme)
int partition(int arr[], int low, int high) {
    int pivot = arr[high]; // Choose last element as pivot
    int i = (low - 1);   // Index of smaller element

    for (int j = low; j < high; j++) {
        // If current element is smaller than or equal to pivot
        if (arr[j] <= pivot) {
            i++; // increment index of smaller element
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

// Find the k-th smallest element (which corresponds to (n-k)th largest index)
int quickselect_kth_smallest(int arr[], int low, int high, int k) {
     // k is 1-based index here, converting to 0-based
    int target_idx = k - 1;

    while (low <= high) {
        int pivot_index = partition(arr, low, high);

        if (pivot_index == target_idx) {
            return arr[pivot_index];
        } else if (pivot_index < target_idx) {
            low = pivot_index + 1;
        } else {
            high = pivot_index - 1;
        }
    }
    return -1; // Should not happen if k is valid
}


void find_top_k_quickselect(int numbers[], int n, int k, int result[]) {
     // Create a copy to partition in place
    int* temp_numbers = (int*)malloc(n * sizeof(int));
    if (!temp_numbers) { perror("malloc quickselect failed"); exit(1); }
    memcpy(temp_numbers, numbers, n * sizeof(int));

    // Find the value of the K-th largest element.
    // This is the (N - K + 1)-th smallest element (using 1-based index for k in quickselect)
    int kth_largest_value = quickselect_kth_smallest(temp_numbers, 0, n - 1, n - k + 1);

    // Collect top K elements (handle duplicates of the threshold value)
    int count = 0;
    for(int i = 0; i < n && count < k; i++) {
        if (numbers[i] >= kth_largest_value) { // Use original numbers array
             result[count++] = numbers[i];
        }
    }
    // If there were many duplicates of kth_largest_value, we might have collected
    // more than k. This simple filter gets *at least* k.
    // A more robust way handles exact counts, but this is sufficient.

    free(temp_numbers);

    // Sort the final K results
    qsort(result, k, sizeof(int), compare_desc);
}


// --- Main Function ---
int main() {
    int* numbers = (int*)malloc(N * sizeof(int));
    int* result = (int*)malloc(K * sizeof(int)); // To store results
     if (!numbers || !result) { perror("malloc main failed"); exit(1); }

    // Generate random numbers
    srand(time(NULL));
    printf("Generating %d numbers...\n", N);
    for (int i = 0; i < N; i++) {
        numbers[i] = rand() % (N * 10); // Range 0 to N*10-1
    }
    printf("Done generating.\n\n");


    clock_t start, end;
    double cpu_time_used;

    // Method 1: Heap
    start = clock();
    find_top_k_heap(numbers, N, K, result);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Method 1 (Heap)       Time: %f seconds\n", cpu_time_used);
    // printf("Result: "); for(int i=0; i<K; ++i) printf("%d ", result[i]); printf("\n");

    // Method 2: Sort
    start = clock();
    find_top_k_sort(numbers, N, K, result);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Method 2 (Sort)       Time: %f seconds\n", cpu_time_used);
    // printf("Result: "); for(int i=0; i<K; ++i) printf("%d ", result[i]); printf("\n");

    // Method 3: Quickselect
    start = clock();
    find_top_k_quickselect(numbers, N, K, result);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Method 3 (Quickselect) Time: %f seconds\n", cpu_time_used);
    // printf("Result: "); for(int i=0; i<K; ++i) printf("%d ", result[i]); printf("\n");


    free(numbers);
    free(result);
    return 0;
}