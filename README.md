# Online Examination System with Adaptive Flow

A simple C-language implementation of an online examination system that uses adaptive question flow through Queue and Binary Tree data structures.

## Features

### Core Functionalities
1. **Queue Management**: Questions are managed using a queue data structure for ordered flow
2. **Adaptive Question Selection**: Binary tree organizes questions by difficulty for adaptive testing
3. **Performance Tracking**: Tracks correct/wrong answers, time spent, and difficulty levels
4. **Exam Taking**: Interactive exam interface with answer validation
5. **Result Evaluation**: Comprehensive result summary with statistics

### Additional Functionalities
6. **Question Bank Viewer**: Display all questions with their statistics
7. **Performance Analysis**: Detailed analysis of performance across difficulty levels
8. **Difficulty Statistics**: View question distribution and success rates by difficulty
9. **Exam History**: Track and view all previous exam attempts

## Data Structures Used

- **Linear**: Queue (for managing question flow during exam)
- **Non-Linear**: Binary Tree (for adaptive question selection by difficulty)

## How to Compile and Run

### Windows
```bash
gcc exam_system.c -o exam_system.exe
exam_system.exe
```

### Linux/Mac
```bash
gcc exam_system.c -o exam_system
./exam_system
```

## How to Use

1. **Main Menu**: Upon starting, you'll see the main menu with 6 options
2. **Take Exam**: Choose option 1 to start an adaptive exam (5 or 10 questions)
3. **View Statistics**: Use options 2-5 to view different statistics and analyses
4. **Navigate**: Answer questions using A/B/C/D or skip with S

## Sample Questions

The system comes pre-loaded with 10 sample questions covering:
- Easy (Mathematics, Geography, History)
- Medium (Science, Literature, Chemistry)
- Hard (Advanced Mathematics, Computer Science, Complexity Theory)

## System Structure

- **Queue Implementation**: FIFO structure for question delivery
- **Binary Tree**: Organizes questions by difficulty
- **Performance Tracking**: Records all attempts and results
- **Statistics**: Real-time calculation of success rates and metrics

## Notes

- Uses basic C language features (arrays, pointers, structures)
- Simple console-based interface
- No external dependencies beyond standard library
- Modular design for easy understanding and modification

