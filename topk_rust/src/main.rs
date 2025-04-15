use rand::Rng;
use std::time::Instant;
use std::collections::BinaryHeap;
use std::cmp::Reverse; // To make BinaryHeap a min-heap

const N: usize = 1000;
const K: usize = 10;

// Method 1: Min-Heap
fn find_top_k_heap(numbers: &[i32]) -> Vec<i32> {
    // Use Reverse to make BinaryHeap act as a min-heap
    let mut min_heap = BinaryHeap::with_capacity(K + 1);

    for &num in numbers.iter() {
        if min_heap.len() < K {
            min_heap.push(Reverse(num));
        } else {
            // Peek gives the largest element (smallest original number)
            if num > min_heap.peek().unwrap().0 {
                 min_heap.pop(); // Remove smallest
                 min_heap.push(Reverse(num)); // Add new larger number
            }
        }
    }

    // Extract elements (will be wrapped in Reverse) and sort descending
    let mut result: Vec<i32> = min_heap.into_iter().map(|rev_num| rev_num.0).collect();
    result.sort_unstable_by(|a, b| b.cmp(a)); // Sort descending
    result
}

// Method 2: Sorting
fn find_top_k_sort(numbers: &[i32]) -> Vec<i32> {
    let mut sorted_numbers = numbers.to_vec();
    // Sort descending
    sorted_numbers.sort_unstable_by(|a, b| b.cmp(a));
    sorted_numbers[0..K].to_vec()
}

// Method 3: Quickselect (using select_nth_unstable)
fn find_top_k_quickselect(numbers: &[i32]) -> Vec<i32> {
    let mut temp_numbers = numbers.to_vec();

    // Partition the slice such that the element at index N-K is the
    // K-th largest element, and all elements after it are >= it.
    temp_numbers.select_nth_unstable(N - K); // Find (N-K)-th smallest = K-th largest position

    // The K largest elements are now in the slice `temp_numbers[N-K..]`
    // They are not necessarily sorted among themselves yet.
    let mut result = temp_numbers[N - K..].to_vec();
    result.sort_unstable_by(|a, b| b.cmp(a)); // Sort the final K elements descending
    result
}


fn main() {
    let mut rng = rand::rng();
    println!("Generating {} numbers...", N);
    let numbers: Vec<i32> = (0..N).map(|_| rng.random_range(0..(N * 10) as i32)).collect();
    println!("Done generating.\n");

    // --- Method 1: Heap ---
    let start = Instant::now();
    let result_heap = find_top_k_heap(&numbers);
    let duration = start.elapsed();
    println!("Method 1 (Heap)       Time: {:?}", duration);
    println!("Result: {:?}", &result_heap[0..K]);


    // --- Method 2: Sort ---
    let start = Instant::now();
    let result_sort = find_top_k_sort(&numbers);
    let duration = start.elapsed();
    println!("Method 2 (Sort)       Time: {:?}", duration);
    println!("Result: {:?}", &result_sort[0..K]);


    // --- Method 3: Quickselect ---
    let start = Instant::now();
    let result_select = find_top_k_quickselect(&numbers);
    let duration = start.elapsed();
    println!("Method 3 (Quickselect) Time: {:?}", duration);
    println!("Result: {:?}", &result_select[0..K]);

    // Verification (optional - check if all results are the same)
    // assert_eq!(result_heap, result_sort);
    // assert_eq!(result_sort, result_select);
    // println!("\nVerification passed (results match)");

}