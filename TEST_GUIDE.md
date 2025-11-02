# Testing Guide for Online Examination System

## Quick Start

1. Run `COMPILE_AND_RUN.bat` (Windows) or compile manually
2. Follow the menu prompts to test different features

## Test Cases

### Test Case 1: Basic Exam Flow
1. Select option 1: "Start Adaptive Exam"
2. Choose "5 Questions Exam"
3. Answer questions with A/B/C/D or skip with S
4. View results and return to menu

### Test Case 2: Question Bank
1. Select option 2: "View Question Bank"
2. Verify all 10 sample questions are displayed
3. Check difficulty levels and statistics

### Test Case 3: Performance Analysis
1. Take an exam first (Test Case 1)
2. Select option 3: "View Performance Analysis"
3. Verify difficulty-based statistics are shown

### Test Case 4: Difficulty Statistics
1. Select option 4: "View Difficulty Statistics"
2. Check question distribution
3. Verify success rates by difficulty

### Test Case 5: Exam History
1. Take 2-3 exams
2. Select option 5: "View Exam History"
3. Verify all attempts are recorded

## Expected Behavior

### Queue Operations
- Questions should be served in FIFO order
- No duplicates should appear
- Queue should empty properly after exam

### Binary Tree Operations
- Questions organized by difficulty
- Tree structure maintained correctly
- No memory leaks

### Statistics
- All calculations should be accurate
- Percentages computed correctly
- History maintained across sessions

## Sample Answers for Testing

For the pre-loaded questions, here are the correct answers:

1. What is 2 + 2? → **B** (4)
2. What is the capital of France? → **B** (Paris)
3. What year did World War II end? → **B** (1945)
4. What is the largest planet? → **C** (Jupiter)
5. Who wrote Romeo and Juliet? → **B** (Shakespeare)
6. What is the chemical symbol for gold? → **C** (Au)
7. What is the square root of 144? → **C** (12)
8. OSI model encryption layer? → **D** (Presentation)
9. Quicksort average complexity? → **B** (O(n log n))
10. Who proved Fermat's Last Theorem? → **C** (Andrew Wiles)

## Troubleshooting

### If compilation fails:
- Ensure GCC is installed
- Check all files are in the same directory
- Verify C standard library is available

### If runtime errors occur:
- Check input format (use single characters A/B/C/D)
- Ensure you press Enter after inputs
- Don't use special characters

### Memory issues:
- System automatically frees tree on exit
- Queue handles deallocation properly
- No manual memory management needed

