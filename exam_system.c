// exam_system_adaptive_fixed.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_QUESTIONS 50
#define NAME_LENGTH 100

// Structure for a question
typedef struct Question {
    int id;
    char question[200];
    char optionA[50];
    char optionB[50];
    char optionC[50];
    char optionD[50];
    char correctAnswer;
    int difficulty; // 1 = Easy, 2 = Medium, 3 = Hard
    int attempts;
    int correctCount;
} Question;

// Queue Node for managing question flow
typedef struct QueueNode {
    Question* question;
    struct QueueNode* next;
} QueueNode;

// Queue structure
typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int count;
} Queue;

// Binary Tree Node for adaptive selection
typedef struct TreeNode {
    Question* question;
    struct TreeNode* left;   // Difficulty increases (harder)
    struct TreeNode* right;  // Difficulty decreases (easier)
} TreeNode;

// Performance tracking structure
typedef struct Performance {
    int questionId;
    char userAnswer;
    char correctAnswer;
    int isCorrect;
    float timeSpent;
} Performance;

// Exam Statistics
typedef struct ExamStats {
    int totalQuestions;
    int correctAnswers;
    int wrongAnswers;
    int skippedQuestions;
    float averageDifficulty;
    float timeTaken;
} ExamStats;

// Global variables
Question questions[MAX_QUESTIONS];
Queue* questionQueue;
TreeNode* adaptiveTree;
Performance performances[MAX_QUESTIONS];
ExamStats currentStats;
int questionCount = 0;
int performanceCount = 0;
int totalExamsTaken = 0;
ExamStats allExams[10];

// Function prototypes
// Queue operations
Queue* createQueue();
void enqueue(Queue* q, Question* question);
Question* dequeue(Queue* q);
int isQueueEmpty(Queue* q);
void flushInput(void);

// Binary Tree operations
TreeNode* createTreeNode(Question* question);
void buildAdaptiveTree(TreeNode** root, Question* questions, int n);
TreeNode* insertQuestion(TreeNode* root, Question* question);
void traverseTree(TreeNode* root, Queue* q); // implemented
void freeTree(TreeNode* root);

// Question operations
void addQuestion(int id, char* q, char* a, char* b, char* c, char* d,
                 char correct, int difficulty);
Question* getQuestionById(int id);
void displayQuestion(Question* q);
void initializeSampleQuestions();

// Performance operations
void trackAnswer(Question* q, char userAnswer);
void evaluateResults();
void displayResults();

// Exam statistics operations
void updateStats(int isCorrect, int difficulty, float time);
void displayStatistics();
void resetStats();
void displayExamHistory();

// Core exam functionalities
void startExam();
void takeAdaptiveExam();

// Extra functionalities
void viewQuestionBank();
void viewPerformanceAnalysis();
void viewDifficultyStatistics();

// Helpers for adaptive logic
TreeNode* findStartNode(TreeNode* root, int preferredDifficulty);
Question* getNextUnaskedQuestion(int difficulty, int usedFlags[]);
Question* getAnyUnaskedQuestion(int usedFlags[]);

int main() {
    int choice;

    printf("=================================================================\n");
    printf("       ONLINE EXAMINATION SYSTEM WITH ADAPTIVE FLOW\n");
    printf("=================================================================\n\n");

    // Initialize
    questionQueue = createQueue();
    adaptiveTree = NULL;
    resetStats();
    initializeSampleQuestions();
    buildAdaptiveTree(&adaptiveTree, questions, questionCount);

    while (1) {
        printf("\n===========================================\n");
        printf("            MAIN MENU\n");
        printf("===========================================\n");
        printf("1. Start Adaptive Exam\n");
        printf("2. View Question Bank\n");
        printf("3. View Performance Analysis\n");
        printf("4. View Difficulty Statistics\n");
        printf("5. View Exam History\n");
        printf("6. Exit\n");
        printf("===========================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            flushInput();
            continue;
        }
        flushInput();

        switch (choice) {
            case 1:
                takeAdaptiveExam();
                break;
            case 2:
                viewQuestionBank();
                break;
            case 3:
                viewPerformanceAnalysis();
                break;
            case 4:
                viewDifficultyStatistics();
                break;
            case 5:
                displayExamHistory();
                break;
            case 6:
                printf("\nThank you for using the system! Goodbye!\n");
                if (adaptiveTree) freeTree(adaptiveTree);
                // Free any remaining queue nodes
                while (!isQueueEmpty(questionQueue)) {
                    dequeue(questionQueue);
                }
                free(questionQueue);
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }

    return 0;
}

// Helper: flush rest of line from stdin
void flushInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// Queue Implementation
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

void enqueue(Queue* q, Question* question) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->question = question;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->count++;
}

Question* dequeue(Queue* q) {
    if (isQueueEmpty(q)) {
        return NULL;
    }

    QueueNode* temp = q->front;
    Question* question = temp->question;
    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->count--;
    return question;
}

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

// Binary Tree Implementation
TreeNode* createTreeNode(Question* question) {
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    node->question = question;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void buildAdaptiveTree(TreeNode** root, Question* questions, int n) {
    int i;
    for (i = 0; i < n; i++) {
        *root = insertQuestion(*root, &questions[i]);
    }
}

TreeNode* insertQuestion(TreeNode* root, Question* question) {
    if (root == NULL) {
        return createTreeNode(question);
    }

    // Insert based on difficulty: left for harder (bigger), right for easier (smaller or equal)
    if (question->difficulty > root->question->difficulty) {
        root->left = insertQuestion(root->left, question);
    } else {
        root->right = insertQuestion(root->right, question);
    }

    return root;
}

// inorder traversal enqueues the questions (used for inspection or fallback)
void traverseTree(TreeNode* root, Queue* q) {
    if (root == NULL) return;
    traverseTree(root->left, q);
    enqueue(q, root->question);
    traverseTree(root->right, q);
}

void freeTree(TreeNode* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// Question Management
void addQuestion(int id, char* q, char* a, char* b, char* c, char* d,
                 char correct, int difficulty) {
    if (questionCount >= MAX_QUESTIONS) {
        printf("Question bank is full!\n");
        return;
    }

    questions[questionCount].id = id;
    strncpy(questions[questionCount].question, q, sizeof(questions[questionCount].question)-1);
    questions[questionCount].question[sizeof(questions[questionCount].question)-1] = '\0';
    strncpy(questions[questionCount].optionA, a, sizeof(questions[questionCount].optionA)-1);
    questions[questionCount].optionA[sizeof(questions[questionCount].optionA)-1] = '\0';
    strncpy(questions[questionCount].optionB, b, sizeof(questions[questionCount].optionB)-1);
    questions[questionCount].optionB[sizeof(questions[questionCount].optionB)-1] = '\0';
    strncpy(questions[questionCount].optionC, c, sizeof(questions[questionCount].optionC)-1);
    questions[questionCount].optionC[sizeof(questions[questionCount].optionC)-1] = '\0';
    strncpy(questions[questionCount].optionD, d, sizeof(questions[questionCount].optionD)-1);
    questions[questionCount].optionD[sizeof(questions[questionCount].optionD)-1] = '\0';
    questions[questionCount].correctAnswer = toupper((unsigned char)correct);
    questions[questionCount].difficulty = difficulty;
    questions[questionCount].attempts = 0;
    questions[questionCount].correctCount = 0;

    questionCount++;
}

void initializeSampleQuestions() {
    // Easy Questions
    addQuestion(1, "What is 2 + 2?", "3", "4", "5", "6", 'B', 1);
    addQuestion(2, "What is the capital of France?", "London", "Paris", "Berlin", "Madrid", 'B', 1);
    addQuestion(3, "What year did World War II end?", "1944", "1945", "1946", "1947", 'B', 2);

    // Medium Questions
    addQuestion(4, "What is the largest planet in our solar system?", "Earth", "Mars", "Jupiter", "Saturn", 'C', 2);
    addQuestion(5, "Who wrote Romeo and Juliet?", "Charles Dickens", "William Shakespeare", "Mark Twain", "Jane Austen", 'B', 2);
    addQuestion(6, "What is the chemical symbol for gold?", "Go", "Gd", "Au", "Ag", 'C', 2);

    // Hard Questions
    addQuestion(7, "What is the square root of 144?", "10", "11", "12", "13", 'C', 3);
    addQuestion(8, "In which layer of the OSI model does encryption occur?", "Physical", "Data Link", "Network", "Presentation", 'D', 3);
    addQuestion(9, "What is the time complexity of quicksort in average case?", "O(n)", "O(n log n)", "O(n^2)", "O(log n)", 'B', 3);
    addQuestion(10, "Who proved Fermat's Last Theorem?", "Einstein", "Gauss", "Andrew Wiles", "Euler", 'C', 3);
}

Question* getQuestionById(int id) {
    int i;
    for (i = 0; i < questionCount; i++) {
        if (questions[i].id == id) {
            return &questions[i];
        }
    }
    return NULL;
}

void displayQuestion(Question* q) {
    printf("\n---------------------------------------\n");
    printf("Question ID: %d\n", q->id);
    printf("Difficulty: %s\n", q->difficulty == 1 ? "Easy" :
                               q->difficulty == 2 ? "Medium" : "Hard");
    printf("\n%s\n", q->question);
    printf("A) %s\n", q->optionA);
    printf("B) %s\n", q->optionB);
    printf("C) %s\n", q->optionC);
    printf("D) %s\n", q->optionD);
    printf("---------------------------------------\n");
}

// Performance Tracking
void trackAnswer(Question* q, char userAnswer) {
    if (performanceCount >= MAX_QUESTIONS) {
        return;
    }

    performances[performanceCount].questionId = q->id;
    performances[performanceCount].userAnswer = toupper((unsigned char)userAnswer);
    performances[performanceCount].correctAnswer = q->correctAnswer;
    performances[performanceCount].isCorrect = (performances[performanceCount].userAnswer == q->correctAnswer);
    performances[performanceCount].timeSpent = 1.0f; // Simplified time tracking

    q->attempts++;
    if (performances[performanceCount].isCorrect) {
        q->correctCount++;
    }

    updateStats(performances[performanceCount].isCorrect, q->difficulty, 1.0f);
    performanceCount++;
}

void evaluateResults() {
    printf("\n\n===========================================\n");
    printf("          EXAM RESULTS SUMMARY\n");
    printf("===========================================\n");
    printf("Total Questions: %d\n", currentStats.totalQuestions);
    printf("Correct Answers: %d\n", currentStats.correctAnswers);
    printf("Wrong Answers: %d\n", currentStats.wrongAnswers);
    printf("Skipped Questions: %d\n", currentStats.skippedQuestions);
    if (currentStats.totalQuestions > 0) {
        printf("Average Score: %.2f%%\n",
               (float)currentStats.correctAnswers / currentStats.totalQuestions * 100.0f);
    } else {
        printf("Average Score: N/A\n");
    }
    printf("Average Difficulty: %.2f\n", currentStats.averageDifficulty);
    printf("===========================================\n");

    // Store exam in history
    if (totalExamsTaken < 10) {
        allExams[totalExamsTaken] = currentStats;
        totalExamsTaken++;
    }
}

void displayResults() {
    int i;
    printf("\n\n===========================================\n");
    printf("          DETAILED RESULTS\n");
    printf("===========================================\n");

    for (i = 0; i < performanceCount; i++) {
        printf("Question %d: ", performances[i].questionId);
        printf("Your answer: %c, ", performances[i].userAnswer);
        printf("Correct: %c, ", performances[i].correctAnswer);
        printf("Status: %s\n", performances[i].isCorrect ? "CORRECT" : "WRONG");
    }
}

// Statistics Management
void resetStats() {
    currentStats.totalQuestions = 0;
    currentStats.correctAnswers = 0;
    currentStats.wrongAnswers = 0;
    currentStats.skippedQuestions = 0;
    currentStats.averageDifficulty = 0.0f;
    currentStats.timeTaken = 0.0f;
    performanceCount = 0;
}

void updateStats(int isCorrect, int difficulty, float time) {
    currentStats.totalQuestions++;
    currentStats.timeTaken += time;

    if (isCorrect) {
        currentStats.correctAnswers++;
    } else {
        currentStats.wrongAnswers++;
    }

    // Update average difficulty incrementally
    currentStats.averageDifficulty =
        ((currentStats.averageDifficulty * (currentStats.totalQuestions - 1)) + difficulty) /
        currentStats.totalQuestions;
}

// Helpers for adaptive logic
TreeNode* findStartNode(TreeNode* root, int preferredDifficulty) {
    if (root == NULL) return NULL;
    if (root->question->difficulty == preferredDifficulty) return root;
    TreeNode* leftSearch = findStartNode(root->left, preferredDifficulty);
    if (leftSearch) return leftSearch;
    return findStartNode(root->right, preferredDifficulty);
}

Question* getNextUnaskedQuestion(int difficulty, int usedFlags[]) {
    for (int i = 0; i < questionCount; ++i) {
        if (!usedFlags[i] && questions[i].difficulty == difficulty) {
            return &questions[i];
        }
    }
    return NULL;
}

Question* getAnyUnaskedQuestion(int usedFlags[]) {
    for (int i = 0; i < questionCount; ++i) {
        if (!usedFlags[i]) return &questions[i];
    }
    return NULL;
}

// Main adaptive exam functionality
void takeAdaptiveExam() {
    int examChoice;
    printf("\n===========================================\n");
    printf("     ADAPTIVE EXAMINATION MODE\n");
    printf("===========================================\n");
    printf("1. 5 Questions Exam\n");
    printf("2. 10 Questions Exam\n");
    printf("Enter your choice: ");

    if (scanf("%d", &examChoice) != 1) {
        printf("Invalid input. Returning to menu.\n");
        flushInput();
        return;
    }
    flushInput();

    int numQuestions = (examChoice == 1) ? 5 : 10;
    if (numQuestions <= 0) numQuestions = 5;

    // Reset
    while (!isQueueEmpty(questionQueue)) dequeue(questionQueue);
    resetStats();

    // Build a queue view of the tree (useful for debugging or non-adaptive flow)
    traverseTree(adaptiveTree, questionQueue);

    // usedFlags to avoid repeating questions in the exam
    int usedFlags[MAX_QUESTIONS] = {0};

    // Find a start node (prefer medium difficulty)
    TreeNode* currentNode = findStartNode(adaptiveTree, 2);
    if (currentNode == NULL) {
        // fallback to root of built tree if no medium found
        currentNode = adaptiveTree;
    }

    // If tree has no nodes, fallback to scanning question bank
    if (currentNode == NULL) {
        // fallback: enqueue first numQuestions questions
        for (int i = 0; i < questionCount && i < numQuestions; ++i) {
            enqueue(questionQueue, &questions[i]);
        }
        printf("\nFallback (no tree) — using question bank order.\n");
    }

    printf("\nExam starting! Good luck!\n\n");

    int asked = 0;
    while (asked < numQuestions) {
        Question* q = NULL;

        if (currentNode != NULL && currentNode->question != NULL && !usedFlags[currentNode->question->id - 1]) {
            q = currentNode->question;
        } else {
            // Try to get unasked question of same difficulty as current node desires
            int targetDiff = 2;
            if (currentNode) targetDiff = currentNode->question->difficulty;
            q = getNextUnaskedQuestion(targetDiff, usedFlags);
            if (q == NULL) q = getAnyUnaskedQuestion(usedFlags);
            if (q == NULL) break; // no questions remaining
        }

        if (q == NULL) break;

        displayQuestion(q);
        printf("Enter your answer (A/B/C/D) or S to skip: ");
        char answer = 0;
        if (scanf(" %c", &answer) != 1) {
            printf("Invalid input. Skipping question.\n");
            flushInput();
            currentStats.skippedQuestions++;
            updateStats(0, q->difficulty, 1.0f);
            usedFlags[q->id - 1] = 1;
            asked++;
            continue;
        }
        flushInput();

        if (toupper((unsigned char)answer) == 'S') {
            printf("Question skipped!\n");
            currentStats.skippedQuestions++;
            updateStats(0, q->difficulty, 1.0f);
            usedFlags[q->id - 1] = 1;
            asked++;
            // keep currentNode unchanged so next question follows same trend
            continue;
        }

        // Track and respond
        trackAnswer(q, answer);
        if (performances[performanceCount - 1].isCorrect) {
            printf("\nCorrect! Well done!\n");
            // Adaptive: move to easier (right)
            if (currentNode && currentNode->right && !usedFlags[currentNode->right->question->id - 1]) {
                currentNode = currentNode->right;
            } else {
                // fallback: pick any unasked question with lower difficulty (if available)
                if (q->difficulty > 1) {
                    Question* fallback = getNextUnaskedQuestion(q->difficulty - 1, usedFlags);
                    if (fallback) {
                        // find node corresponding to fallback
                        // simple approach: set currentNode NULL and rely on direct selection next loop
                        currentNode = NULL;
                    } else {
                        currentNode = NULL;
                    }
                } else {
                    currentNode = NULL;
                }
            }
        } else {
            printf("\nWrong! Correct answer was %c\n", q->correctAnswer);
            // Adaptive: move to harder (left)
            if (currentNode && currentNode->left && !usedFlags[currentNode->left->question->id - 1]) {
                currentNode = currentNode->left;
            } else {
                // fallback: pick unasked question with higher difficulty
                if (q->difficulty < 3) {
                    Question* fallback = getNextUnaskedQuestion(q->difficulty + 1, usedFlags);
                    if (fallback) {
                        currentNode = NULL;
                    } else {
                        currentNode = NULL;
                    }
                } else {
                    currentNode = NULL;
                }
            }
        }

        usedFlags[q->id - 1] = 1;
        asked++;

        printf("\nPress Enter to continue...");
        getchar();
    }

    // Display results
    evaluateResults();
    displayResults();

    printf("\nPress Enter to return to main menu...");
    getchar();
}

// Extra Functionality 1: View Question Bank
void viewQuestionBank() {
    int i;
    printf("\n\n===========================================\n");
    printf("          QUESTION BANK\n");
    printf("===========================================\n");
    printf("Total Questions: %d\n\n", questionCount);

    for (i = 0; i < questionCount; i++) {
        printf("Question %d:\n", i + 1);
        displayQuestion(&questions[i]);
        printf("Attempts: %d | Correct: %d | Success Rate: %.1f%%\n\n",
               questions[i].attempts,
               questions[i].correctCount,
               questions[i].attempts > 0 ?
               (float)questions[i].correctCount / questions[i].attempts * 100.0f : 0.0f);
    }

    printf("Press Enter to continue...");
    getchar();
}

// Extra Functionality 2: Performance Analysis
void viewPerformanceAnalysis() {
    int i;
    int easyCorrect = 0, easyAttempted = 0;
    int mediumCorrect = 0, mediumAttempted = 0;
    int hardCorrect = 0, hardAttempted = 0;

    if (totalExamsTaken == 0 && performanceCount == 0) {
        printf("\nNo exam history available yet!\n");
        printf("Press Enter to continue...");
        getchar();
        return;
    }

    // Calculate overall performance by difficulty
    for (i = 0; i < questionCount; i++) {
        if (questions[i].difficulty == 1) {
            easyAttempted += questions[i].attempts;
            easyCorrect += questions[i].correctCount;
        } else if (questions[i].difficulty == 2) {
            mediumAttempted += questions[i].attempts;
            mediumCorrect += questions[i].correctCount;
        } else if (questions[i].difficulty == 3) {
            hardAttempted += questions[i].attempts;
            hardCorrect += questions[i].correctCount;
        }
    }

    printf("\n\n===========================================\n");
    printf("       PERFORMANCE ANALYSIS\n");
    printf("===========================================\n");

    if (easyAttempted > 0) {
        printf("Easy Questions:\n");
        printf("  Attempted: %d | Correct: %d | Success Rate: %.1f%%\n",
               easyAttempted, easyCorrect, (float)easyCorrect / easyAttempted * 100.0f);
    }

    if (mediumAttempted > 0) {
        printf("\nMedium Questions:\n");
        printf("  Attempted: %d | Correct: %d | Success Rate: %.1f%%\n",
               mediumAttempted, mediumCorrect, (float)mediumCorrect / mediumAttempted * 100.0f);
    }

    if (hardAttempted > 0) {
        printf("\nHard Questions:\n");
        printf("  Attempted: %d | Correct: %d | Success Rate: %.1f%%\n",
               hardAttempted, hardCorrect, (float)hardCorrect / hardAttempted * 100.0f);
    }

    printf("\n===========================================\n");

    // Overall performance across all exams
    printf("\nOverall Exam Performance:\n");
    printf("Total Exams Taken: %d\n", totalExamsTaken);

    if (totalExamsTaken > 0) {
        float totalScore = 0;
        for (i = 0; i < totalExamsTaken; i++) {
            if (allExams[i].totalQuestions > 0) {
                totalScore += (float)allExams[i].correctAnswers / allExams[i].totalQuestions * 100.0f;
            }
        }
        printf("Average Score Across All Exams: %.2f%%\n", totalScore / totalExamsTaken);
    } else {
        printf("Average Score Across All Exams: N/A\n");
    }
    printf("===========================================\n");

    printf("\nPress Enter to continue...");
    getchar();
}

// Extra Functionality 3: Difficulty Statistics
void viewDifficultyStatistics() {
    int i;
    int easyCount = 0, mediumCount = 0, hardCount = 0;

    printf("\n\n===========================================\n");
    printf("       DIFFICULTY STATISTICS\n");
    printf("===========================================\n");

    for (i = 0; i < questionCount; i++) {
        if (questions[i].difficulty == 1) easyCount++;
        else if (questions[i].difficulty == 2) mediumCount++;
        else if (questions[i].difficulty == 3) hardCount++;
    }

    if (questionCount == 0) {
        printf("No questions in bank.\n");
    } else {
        printf("Question Bank Distribution:\n");
        printf("Easy Questions: %d (%.1f%%)\n", easyCount, (float)easyCount / questionCount * 100.0f);
        printf("Medium Questions: %d (%.1f%%)\n", mediumCount, (float)mediumCount / questionCount * 100.0f);
        printf("Hard Questions: %d (%.1f%%)\n", hardCount, (float)hardCount / questionCount * 100.0f);
    }

    printf("\n===========================================\n");
    printf("Question Performance by Difficulty:\n\n");

    for (i = 0; i < questionCount; i++) {
        char diffStr[10];
        if (questions[i].difficulty == 1) strcpy(diffStr, "Easy");
        else if (questions[i].difficulty == 2) strcpy(diffStr, "Medium");
        else strcpy(diffStr, "Hard");

        printf("Q%d (%s): ", questions[i].id, diffStr);
        if (questions[i].attempts > 0) {
            printf("Success Rate: %.1f%% (%d/%d)\n",
                   (float)questions[i].correctCount / questions[i].attempts * 100.0f,
                   questions[i].correctCount, questions[i].attempts);
        } else {
            printf("Not yet attempted\n");
        }
    }

    printf("===========================================\n");

    printf("\nPress Enter to continue...");
    getchar();
}

// Extra Functionality 4: Exam History
void displayExamHistory() {
    int i;

    printf("\n\n===========================================\n");
    printf("          EXAM HISTORY\n");
    printf("===========================================\n");

    if (totalExamsTaken == 0) {
        printf("No exams taken yet!\n");
    } else {
        for (i = 0; i < totalExamsTaken; i++) {
            printf("\nExam %d:\n", i + 1);
            printf("  Total Questions: %d\n", allExams[i].totalQuestions);
            printf("  Correct Answers: %d\n", allExams[i].correctAnswers);
            if (allExams[i].totalQuestions > 0) {
                printf("  Score: %.1f%%\n",
                       (float)allExams[i].correctAnswers / allExams[i].totalQuestions * 100.0f);
            } else {
                printf("  Score: N/A\n");
            }
            printf("  Average Difficulty: %.2f\n", allExams[i].averageDifficulty);
            printf("---------------------------------------\n");
        }
    }

    printf("\nPress Enter to continue...");
    getchar();
}
