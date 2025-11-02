#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    struct TreeNode* left;   // Difficulty increases (wrong answer)
    struct TreeNode* right;  // Difficulty decreases (correct answer)
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
void displayQueue(Queue* q);

// Binary Tree operations
TreeNode* createTreeNode(Question* question);
void buildAdaptiveTree(TreeNode** root, Question* questions, int n);
TreeNode* insertQuestion(TreeNode* root, Question* question);
void traverseTree(TreeNode* root, Queue* q);
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
        scanf("%d", &choice);
        
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
                exit(0);
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
    }
    
    return 0;
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
    
    // Insert based on difficulty: left for harder, right for easier
    if (question->difficulty > root->question->difficulty) {
        root->left = insertQuestion(root->left, question);
    } else {
        root->right = insertQuestion(root->right, question);
    }
    
    return root;
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
    strcpy(questions[questionCount].question, q);
    strcpy(questions[questionCount].optionA, a);
    strcpy(questions[questionCount].optionB, b);
    strcpy(questions[questionCount].optionC, c);
    strcpy(questions[questionCount].optionD, d);
    questions[questionCount].correctAnswer = correct;
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
    performances[performanceCount].userAnswer = userAnswer;
    performances[performanceCount].correctAnswer = q->correctAnswer;
    performances[performanceCount].isCorrect = (userAnswer == q->correctAnswer);
    performances[performanceCount].timeSpent = 1.0; // Simplified time tracking
    
    q->attempts++;
    if (performances[performanceCount].isCorrect) {
        q->correctCount++;
    }
    
    updateStats(performances[performanceCount].isCorrect, q->difficulty, 1.0);
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
    printf("Average Score: %.2f%%\n", 
           (float)currentStats.correctAnswers / currentStats.totalQuestions * 100);
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
    currentStats.averageDifficulty = 0.0;
    currentStats.timeTaken = 0.0;
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
    
    // Update average difficulty
    currentStats.averageDifficulty = 
        ((currentStats.averageDifficulty * (currentStats.totalQuestions - 1)) + difficulty) / 
        currentStats.totalQuestions;
}

// Main Exam Functionality
void takeAdaptiveExam() {
    int examChoice;
    printf("\n===========================================\n");
    printf("     ADAPTIVE EXAMINATION MODE\n");
    printf("===========================================\n");
    printf("1. 5 Questions Exam\n");
    printf("2. 10 Questions Exam\n");
    printf("Enter your choice: ");
    scanf("%d", &examChoice);
    
    int numQuestions = (examChoice == 1) ? 5 : 10;
    
    // Clear previous queue and performances
    while (!isQueueEmpty(questionQueue)) {
        dequeue(questionQueue);
    }
    resetStats();
    
    // Traverse tree and populate queue with questions
    printf("\nBuilding adaptive question flow...\n");
    
    // Simplified: Add questions to queue (in a real system, this would be adaptive based on performance)
    int i, qAdded = 0;
    int startDiff = 2; // Start with medium difficulty
    
    for (i = 0; i < questionCount && qAdded < numQuestions; i++) {
        if (questions[i].difficulty == startDiff || 
            (qAdded % 2 == 0 && questions[i].difficulty < startDiff) ||
            (qAdded % 3 == 0 && questions[i].difficulty > startDiff)) {
            enqueue(questionQueue, &questions[i]);
            qAdded++;
        }
    }
    
    printf("\nExam starting! Good luck!\n\n");
    
    // Take exam
    int questionNum = 1;
    while (!isQueueEmpty(questionQueue) && questionNum <= numQuestions) {
        Question* q = dequeue(questionQueue);
        
        if (q == NULL) break;
        
        printf("Question %d of %d:", questionNum, numQuestions);
        displayQuestion(q);
        
        printf("Enter your answer (A/B/C/D) or S to skip: ");
        char answer;
        scanf(" %c", &answer);
        
        if (answer == 'S' || answer == 's') {
            printf("Question skipped!\n");
            currentStats.skippedQuestions++;
            updateStats(0, q->difficulty, 1.0);
        } else {
            trackAnswer(q, answer);
            
            if (answer == q->correctAnswer) {
                printf("\nCorrect! Well done!\n");
            } else {
                printf("\nWrong! Correct answer was %c\n", q->correctAnswer);
            }
        }
        
        questionNum++;
        printf("\nPress Enter to continue...");
        getchar();
        getchar();
    }
    
    // Display results
    evaluateResults();
    displayResults();
    
    printf("\nPress Enter to return to main menu...");
    getchar();
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
               (float)questions[i].correctCount / questions[i].attempts * 100 : 0.0);
    }
    
    printf("Press Enter to continue...");
    getchar();
    getchar();
}

// Extra Functionality 2: Performance Analysis
void viewPerformanceAnalysis() {
    int i;
    int easyCorrect = 0, easyAttempted = 0;
    int mediumCorrect = 0, mediumAttempted = 0;
    int hardCorrect = 0, hardAttempted = 0;
    
    if (totalExamsTaken == 0) {
        printf("\nNo exam history available yet!\n");
        printf("Press Enter to continue...");
        getchar();
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
               easyAttempted, easyCorrect, (float)easyCorrect / easyAttempted * 100);
    }
    
    if (mediumAttempted > 0) {
        printf("\nMedium Questions:\n");
        printf("  Attempted: %d | Correct: %d | Success Rate: %.1f%%\n",
               mediumAttempted, mediumCorrect, (float)mediumCorrect / mediumAttempted * 100);
    }
    
    if (hardAttempted > 0) {
        printf("\nHard Questions:\n");
        printf("  Attempted: %d | Correct: %d | Success Rate: %.1f%%\n",
               hardAttempted, hardCorrect, (float)hardCorrect / hardAttempted * 100);
    }
    
    printf("\n===========================================\n");
    
    // Overall performance across all exams
    printf("\nOverall Exam Performance:\n");
    printf("Total Exams Taken: %d\n", totalExamsTaken);
    
    float totalScore = 0;
    for (i = 0; i < totalExamsTaken; i++) {
        totalScore += (float)allExams[i].correctAnswers / allExams[i].totalQuestions * 100;
    }
    
    printf("Average Score Across All Exams: %.2f%%\n", totalScore / totalExamsTaken);
    printf("===========================================\n");
    
    printf("\nPress Enter to continue...");
    getchar();
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
    
    printf("Question Bank Distribution:\n");
    printf("Easy Questions: %d (%.1f%%)\n", easyCount, (float)easyCount / questionCount * 100);
    printf("Medium Questions: %d (%.1f%%)\n", mediumCount, (float)mediumCount / questionCount * 100);
    printf("Hard Questions: %d (%.1f%%)\n", hardCount, (float)hardCount / questionCount * 100);
    
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
                   (float)questions[i].correctCount / questions[i].attempts * 100,
                   questions[i].correctCount, questions[i].attempts);
        } else {
            printf("Not yet attempted\n");
        }
    }
    
    printf("===========================================\n");
    
    printf("\nPress Enter to continue...");
    getchar();
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
            printf("  Score: %.1f%%\n", 
                   (float)allExams[i].correctAnswers / allExams[i].totalQuestions * 100);
            printf("  Average Difficulty: %.2f\n", allExams[i].averageDifficulty);
            printf("---------------------------------------\n");
        }
    }
    
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

