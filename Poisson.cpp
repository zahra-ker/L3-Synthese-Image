#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <jerror.h>
#include <cstdlib>
#include <vector>
#define PI 3.14159265358979323846

// Structure pour la texture
struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};
typedef struct Image Image;

/* Classe Point pour stocker les sommets */
class Point {
public:
    // Coordonnées x, y et z du point
    double x;
    double y;
    double z;
    // Couleur r, v et b du point
    float r;
    float g;
    float b;
};

// Variables globales
char presse;
int anglex = 52, angley = 22, x, y, xold, yold;
float zoom = -12.0; // Variable pour gérer la distance entre la caméra et le poisson
float angleNageoires = 0.0f;  // Angle de rotation des nageoires
float angleQueue = 0.0f;  // Angle de rotation de la queue
bool animationActive = false;  // Etat de l'animation
bool sensAnimation = true;     // Direction de l'animation
bool animationQueueActive = true;  // La queue est animée par défaut (animation automatique)
static bool timerStarted = false;

// Pour les textures et le fond
GLuint textureID;
Image *image;
GLuint oeilTextureID;

// Pour la couleur du fond et la gestion de la lumière sur celui-ci
float fondR = 0.082;
float fondG = 0.376;
float fondB = 0.741;
float fondDarkFactor = 0.2; // Facteur d'assombrissement quand la lumière est éteinte
bool light0On = true;  // État de la lumière principale
bool light1On = false; // État du spot
float fondBrightFactor = 1.3; // Facteur d'éclaircissement quand les deux lumières sont allumées
float fondNormalFactor = 1.0; // Facteur normal


// Points des faces du poisson
Point pPoisson[141] = {
    {0.38, 0.49, -0.49, 1.0, 0.0, 0.0}, // v1
    {0.38, -0.49, -0.49, 0.0, 1.0, 0.0}, // v2
    {0.38, 0.49, 0.49, 0.0, 0.0, 1.0}, // v3
    {0.38, -0.49, 0.49, 1.0, 1.0, 1.0}, // v4
    {-0.38, 0.49, -0.49, 1.0, 0.0, 0.0}, // v5
    {-0.38, -0.49, -0.49, 0.0, 1.0, 0.0}, // v6
    {-0.38, 0.49, 0.49, 0.0, 0.0, 1.0}, // v7
    {-0.38, -0.49, 0.49, 1.0, 1.0, 1.0}, // v8
    {-0.17, -0.22, 0.83, 1.0, 0.0, 0.0}, // v9
    {-0.17, 0.22, 0.83, 0.0, 1.0, 0.0}, // v10
    {0.17, 0.22, 0.83, 0.0, 0.0, 1.0}, // v11
    {0.17, -0.22, 0.83, 1.0, 1.0, 1.0}, // v12
    {0.25, -0.325, -1.27, 1.0, 0.0, 0.0}, // v13
    {-0.25, -0.325, -1.27, 0.0, 1.0, 0.0}, // v14
    {0.25, 0.325, -1.27, 0.0, 0.0, 1.0}, // v15
    {-0.25, 0.325, -1.27, 1.0, 1.0, 1.0}, // v16
    {0.14, -0.185, -1.80, 1.0, 0.0, 0.0}, // v17
    {-0.14, -0.185, -1.80, 0.0, 1.0, 0.0}, // v18
    {0.14, 0.185, -1.80, 0.0, 0.0, 1.0}, // v19
    {-0.14, 0.185, -1.80, 1.0, 1.0, 1.0}, // v20
    {-0.42, 0.55, 0.00, 1.0, 0.0, 0.0}, // v21
    {-0.42, -0.55, 0.00, 0.0, 1.0, 0.0}, // v22
    {0.42, -0.55, 0.00, 0.0, 0.0, 1.0}, // v23
    {0.42, 0.55, 0.00, 1.0, 1.0, 1.0}, // v24
    {0.00, -0.54, -0.51, 1.0, 0.0, 0.0}, // v25
    {0.00, 0.54, 0.58, 0.0, 1.0, 0.0}, // v26
    {0.00, -0.54, 0.58, 0.0, 0.0, 1.0}, // v27
    {-0.07, 0.80, 0.04, 1.0, 1.0, 1.0}, // v28
    {0.00, 0.25, 0.91, 1.0, 0.0, 0.0}, // v29
    {0.00, -0.25, 0.91, 0.0, 1.0, 0.0}, // v30
    {0.00, 0.36, -1.37, 0.0, 0.0, 1.0}, // v31
    {0.00, -0.14, -1.92, 1.0, 1.0, 1.0}, // v32
    {0.00, 0.14, -1.92, 1.0, 0.0, 0.0}, // v33
    {0.00, -0.61, 0.04, 0.0, 1.0, 0.0}, // v34
    {0.00, 0.82, 0.09, 0.0, 0.0, 1.0}, // v35
    {0.42, 0.00, -0.49, 1.0, 1.0, 1.0}, // v36
    {-0.42, 0.00, 0.49, 1.0, 0.0, 0.0}, // v37
    {0.42, 0.00, 0.49, 0.0, 1.0, 0.0}, // v38
    {-0.42, 0.00, -0.49, 0.0, 0.0, 1.0}, // v39
    {-0.19, 0.00, 0.83, 1.0, 1.0, 1.0}, // v40
    {0.19, 0.00, 0.83, 1.0, 0.0, 0.0}, // v41
    {0.28, 0.00, -1.27, 0.0, 1.0, 0.0}, // v42
    {-0.28, 0.00, -1.27, 0.0, 0.0, 1.0}, // v43
    {0.12, 0.00, -1.88, 1.0, 1.0, 1.0}, // v44
    {-0.12, 0.00, -1.88, 1.0, 0.0, 0.0}, // v45
    {0.47, 0.00, 0.00, 0.0, 1.0, 0.0}, // v46
    {-0.47, 0.00, 0.00, 0.0, 0.0, 1.0}, // v47
    {0.00, 0.00, -1.96, 1.0, 1.0, 1.0}, // v48
    {0.00, 0.70, 0.41, 1.0, 0.0, 0.0}, // v49
    {-0.13, 0.65, 0.34, 0.0, 1.0, 0.0}, // v50
    {-0.13, 0.65, -0.48, 0.0, 0.0, 1.0}, // v51
    {-0.14, 0.71, -0.07, 1.0, 1.0, 1.0}, // v52
    {-0.11, 0.07, 1.04, 1.0, 0.0, 0.0}, // v53
    {0.13, 0.65, -0.48, 0.0, 1.0, 0.0}, // v54
    {0.00, 0.51, -1.21, 0.0, 0.0, 1.0}, // v55
    {0.08, 0.48, -1.13, 1.0, 1.0, 1.0}, // v56
    {-0.08, 0.48, -1.13, 1.0, 0.0, 0.0}, // v57
    {0.14, 0.71, -0.07, 0.0, 1.0, 0.0}, // v58
    {0.13, 0.65, 0.34, 0.0, 0.0, 1.0}, // v59
    {-0.11, 0.00, 1.04, 1.0, 1.0, 1.0}, // v60
    {-0.07, 0.80, -0.43, 1.0, 0.0, 0.0}, // v61
    {-0.08, 0.83, -0.20, 0.0, 1.0, 0.0}, // v62
    {0.00, 0.82, -0.44, 0.0, 0.0, 1.0}, // v63
    {0.07, 0.80, -0.43, 1.0, 1.0, 1.0}, // v64
    {0.00, 0.72, -0.86, 1.0, 0.0, 0.0}, // v65
    {0.05, 0.70, -0.81, 0.0, 1.0, 0.0}, // v66
    {-0.05, 0.70, -0.81, 0.0, 0.0, 1.0}, // v67
    {0.08, 0.83, -0.20, 1.0, 1.0, 1.0}, // v68
    {0.07, 0.80, 0.04, 1.0, 0.0, 0.0}, // v69
    {0.00, 0.86, -0.18, 0.0, 1.0, 0.0}, // v70
    {0.11, 0.00, 1.04, 0.0, 0.0, 1.0}, // v71
    {0.11, -0.07, 1.04, 1.0, 1.0, 1.0}, // v72
    {0.00, 0.08, 1.08, 1.0, 0.0, 0.0}, // v73
    {0.00, -0.08, 1.08, 0.0, 1.0, 0.0}, // v74
    {0.11, 0.07, 1.04, 0.0, 0.0, 1.0}, // v75
    {-0.11, -0.07, 1.04, 1.0, 1.0, 1.0}, // v76
    {-0.01, -0.14, -1.92, 1.0, 0.0, 0.0}, // v77
    {-0.01, 0.14, -1.92, 0.0, 1.0, 0.0}, // v78
    {-0.01, 0.00, -1.96, 0.0, 0.0, 1.0}, // v79
    {-0.02, -0.14, -2.00, 1.0, 1.0, 1.0}, // v80
    {-0.02, 0.14, -2.00, 1.0, 0.0, 0.0}, // v81
    {-0.02, 0.00, -2.04, 0.0, 1.0, 0.0}, // v82
    {-0.03, -0.28, -2.26, 0.0, 0.0, 1.0}, // v83
    {-0.03, 0.28, -2.26, 1.0, 1.0, 1.0}, // v84
    {-0.03, 0.00, -2.30, 1.0, 0.0, 0.0}, // v85
    {-0.02, -0.47, -2.45, 0.0, 1.0, 0.0}, // v86
    {-0.02, 0.47, -2.45, 0.0, 0.0, 1.0}, // v87
    {-0.02, 0.00, -2.49, 1.0, 1.0, 1.0}, // v88
    {-0.62, 0.17, -0.12, 1.0, 0.0, 0.0}, // v89
    {-0.62, -0.17, -0.12, 0.0, 1.0, 0.0}, // v90
    {0.62, -0.17, -0.12, 0.0, 0.0, 1.0}, // v91
    {0.62, 0.17, -0.12, 1.0, 1.0, 1.0}, // v92
    {0.57, 0.00, 0.00, 1.0, 0.0, 0.0}, // v93
    {-0.57, 0.00, 0.00, 0.0, 1.0, 0.0}, // v94
    {0.73, -0.52, -0.43, 0.0, 0.0, 1.0}, // v95
    {0.73, 0.52, -0.43, 1.0, 1.0, 1.0}, // v96
    {0.63, 0.00, -0.59, 1.0, 0.0, 0.0}, // v97
    {-0.73, 0.52, -0.43, 0.0, 1.0, 0.0}, // v98
    {-0.73, -0.52, -0.43, 0.0, 0.0, 1.0}, // v99
    {-0.63, 0.00, -0.59, 1.0, 1.0, 1.0}, // v100
    {0.01, -0.14, -1.92, 1.0, 0.0, 0.0}, // v101
    {0.01, 0.00, -1.96, 0.0, 1.0, 0.0}, // v102
    {0.01, 0.14, -1.92, 0.0, 0.0, 1.0}, // v103
    {0.02, -0.14, -2.00, 1.0, 1.0, 1.0}, // v104
    {0.02, 0.00, -2.04, 1.0, 0.0, 0.0}, // v105
    {0.02, 0.14, -2.00, 0.0, 1.0, 0.0}, // v106
    {0.03, -0.28, -2.26, 0.0, 0.0, 1.0}, // v107
    {0.03, 0.00, -2.30, 1.0, 1.0, 1.0}, // v108
    {0.03, 0.28, -2.26, 1.0, 0.0, 0.0}, // v109
    {0.02, -0.47, -2.45, 0.0, 1.0, 0.0}, // v110
    {0.02, 0.00, -2.49, 0.0, 0.0, 1.0}, // v111
    {0.02, 0.47, -2.45, 1.0, 1.0, 1.0}, // v112
    {-0.47, 0.00, 0.00, 1.0, 0.0, 0.0}, // v113
    {-0.53, -0.17, -0.12, 0.0, 1.0, 0.0}, // v114
    {-0.53, 0.17, -0.12, 0.0, 0.0, 1.0}, // v115
    {-0.54, 0.00, -0.59, 1.0, 1.0, 1.0}, // v116
    {-0.64, -0.52, -0.43, 1.0, 0.0, 0.0}, // v117
    {-0.64, 0.52, -0.43, 0.0, 1.0, 0.0}, // v118
    {0.47, 0.00, 0.00, 0.0, 0.0, 1.0},  // v119
    {0.53, -0.17, -0.12, 1.0, 1.0, 1.0}, // v120
    {0.53, 0.17, -0.12, 1.0, 0.0, 0.0}, // v121
    {0.54, 0.00, -0.59, 0.0, 1.0, 0.0}, // v122
    {0.64, -0.52, -0.43, 0.0, 0.0, 1.0}, // v123
    {0.64, 0.52, -0.43, 1.0, 1.0, 1.0}, // v124
    {-0.00, -0.51, -0.92, 1.0, 0.0, 0.0}, // v125
    {-0.18, -0.48, -0.91, 0.0, 1.0, 0.0}, // v126
    {-0.12, -0.41, -1.27, 0.0, 0.0, 1.0}, // v127
    {0.12, -0.41, -1.27, 1.0, 1.0, 1.0}, // v128
    {0.18, -0.48, -0.91, 1.0, 0.0, 0.0}, // v129
    {-0.00, -0.32, -1.57, 0.0, 1.0, 0.0}, // v130
    {-0.07, -0.34, -1.52, 0.0, 0.0, 1.0}, // v131
    {0.07, -0.34, -1.52, 1.0, 1.0, 1.0}, // v132
    {0.00, -0.59, -1.12, 1.0, 0.0, 0.0}, // v133
    {-0.09, -0.58, -1.12, 0.0, 1.0, 0.0}, // v134
    {0.00, -0.55, -1.32, 0.0, 0.0, 1.0}, // v135
    {-0.06, -0.54, -1.30, 1.0, 1.0, 1.0}, // v136
    {0.06, -0.54, -1.30, 1.0, 0.0, 0.0}, // v137
    {0.09, -0.58, -1.12, 0.0, 1.0, 0.0}, // v138
    {0.00, -0.50, -1.45, 0.0, 0.0, 1.0}, // v139
    {-0.03, -0.51, -1.42, 1.0, 1.0, 1.0}, // v140
    {0.03, -0.51, -1.42, 1.0, 0.0, 0.0} // v141
};

// Faces de la tête du poisson
int fPoissonTete[16][4] = {
    {37, 8, 9, 40},      // 1
    {29, 10, 53, 73},    // 6
    {27, 4, 12, 30},     // 7
    {26, 7, 10, 29},     // 8
    {38, 3, 11, 41},     // 9
    {3, 26, 29, 11},     // 27
    {8, 27, 30, 9},      // 28
    {9, 30, 74, 76},     // 29
    {41, 11, 75, 71},    // 32
    {4, 38, 41, 12},     // 38
    {12, 41, 71, 72},    // 39
    {7, 37, 40, 10},     // 42
    {10, 40, 60, 53},    // 68
    {40, 9, 76, 60},     // 69
    {11, 29, 73, 75},    // 70
    {30, 12, 72, 74}     // 71
};

// Faces de la nageoire gauche du poisson
int fPoissonGauche[10][4] = {
    {93, 97, 96, 92},    // 78
    {93, 97, 95, 91},    // 79
    {119, 121, 124, 122}, // 106
    {119, 120, 123, 122}, // 107
    {95, 97, 122, 123},   // 108
    {93, 92, 121, 119},   // 109
    {93, 91, 120, 119},   // 110
    {92, 96, 124, 121},   // 111
    {96, 97, 122, 124},   // 112
    {91, 95, 123, 120}    // 113
};

// Faces de la nageoire droite du poisson
int fPoissonDroite[10][4] = {
    {94, 100, 98, 89},    // 80
    {94, 100, 99, 90},    // 81
    {113, 115, 118, 116}, // 98
    {113, 114, 117, 116}, // 99
    {94, 89, 115, 113},   // 100
    {89, 98, 118, 115},   // 101
    {94, 90, 114, 113},   // 102
    {98, 100, 116, 118},  // 103
    {90, 99, 117, 114},   // 104
    {99, 100, 116, 117}   // 105
};

// Faces de la queue du poisson
int fPoissonArriere[22][4] = {
    {79, 82, 81, 78},     // 72
    {79, 82, 80, 77},     // 73
    {82, 85, 84, 81},     // 74
    {82, 85, 83, 80},     // 75
    {85, 88, 87, 84},     // 76
    {85, 88, 86, 83},     // 77
    {102, 103, 106, 105}, // 82
    {102, 101, 104, 105}, // 83
    {105, 106, 109, 108}, // 84
    {105, 104, 107, 108}, // 85
    {108, 109, 112, 111}, // 86
    {108, 107, 110, 111}, // 87
    {79, 78, 103, 102},   // 88
    {84, 87, 112, 109},   // 89
    {78, 81, 106, 103},   // 90
    {86, 88, 111, 110},   // 91
    {83, 86, 110, 107},   // 92
    {77, 80, 104, 101},   // 93
    {87, 88, 111, 112},   // 94
    {81, 84, 109, 106},   // 95
    {79, 77, 101, 102},   // 96
    {80, 83, 107, 104}    // 97
};

// Corps principal (donc sans les nageoires, la queue et le visage)
int fPoissonCorps[44][4] = {
   {47, 21, 5, 39},    // 0
   {31, 16, 57, 55},   // 2
   {34, 23, 4, 27},    // 3
   {46, 24, 3, 38},    // 4
   {39, 5, 16, 43},    // 5
   {43, 16, 20, 45},   // 10
   {32, 17, 132, 130}, // 11
   {1, 15, 56, 54},    // 12
   {36, 2, 13, 42},    // 13
   {25, 6, 126, 125},  // 14
   {31, 15, 19, 33},   // 15
   {42, 13, 17, 44},   // 16
   {36, 1, 24, 46},    // 17
   {25, 2, 23, 34},    // 18
   {3, 24, 58, 59},    // 19
   {37, 7, 21, 47},    // 20
   {15, 31, 55, 56},   // 21
   {6, 25, 34, 22},    // 22
   {16, 31, 33, 20},   // 23
   {17, 13, 128, 132}, // 24
   {7, 26, 49, 50},    // 25
   {6, 14, 127, 126},  // 26
   {22, 34, 27, 8},    // 30
   {21, 7, 50, 52},    // 31
   {8, 37, 47, 22},    // 33
   {2, 36, 46, 23},    // 34
   {15, 42, 44, 19},   // 35
   {1, 36, 42, 15},    // 36
   {14, 43, 45, 18},   // 37
   {6, 39, 43, 14},    // 40
   {23, 46, 38, 4},    // 41
   {22, 47, 39, 6},    // 43
   {33, 19, 44, 48},   // 44
   {20, 33, 48, 45},   // 45
   {18, 45, 48, 32},   // 46
   {44, 17, 32, 48},   // 47
   {5, 21, 52, 51},    // 54
   {16, 5, 51, 57},    // 55
   {26, 3, 59, 49},    // 56
   {24, 1, 54, 58},    // 57
   {18, 32, 130, 131},   // 118
   {13, 2, 129, 128},    // 119
   {2, 25, 125, 129},    // 120
   {14, 18, 131, 127}    // 121
};

// Faces de la nageoire supérieure du poisson
int fPoissonNageoireSup[16][4] = {
    {57, 51, 61, 67},    // 48
    {58, 54, 64, 68},    // 49
    {51, 52, 62, 61},    // 50
    {56, 55, 65, 66},    // 51
    {59, 58, 68, 69},    // 52
    {50, 49, 35, 28},    // 53
    {70, 63, 61, 62},    // 58
    {63, 64, 66, 65},    // 59
    {35, 70, 62, 28},    // 60
    {69, 68, 70, 35},    // 61
    {61, 63, 65, 67},    // 62
    {68, 64, 63, 70},    // 63
    {55, 57, 67, 65},    // 64
    {52, 50, 28, 62},    // 65
    {54, 56, 66, 64},    // 66
    {49, 59, 69, 35}     // 67
};

// Faces de la nageoire inférieure du poisson
int fPoissonNageoireInf[12][4] = {
   {128, 129, 138, 137},  // 129
   {132, 128, 137, 141},  // 117
   {130, 132, 141, 139},  // 126
   {131, 130, 139, 140},  // 116
   {127, 131, 140, 136},  // 128
   {126, 127, 136, 134},  // 115
   {125, 126, 134, 133},  // 114
   {129, 125, 133, 138},  // 127
   {137, 135, 139, 141},  // 124
   {135, 136, 140, 139},  // 123
   {138, 133, 135, 137},  // 125
   {133, 134, 136, 135}   // 122
};

//Prototype des fonctions
void affichage();
void clavier(unsigned char touche, int x, int y);
void special(int key, int x, int y); // gestion de la caméra avec les touches directionnelles
void reshape(int x, int y);
void mouse(int bouton, int etat, int x, int y);
void mousemotion(int x, int y);

// Fonction pour charger une image JPEG
Image* loadJPEG(const char *filename) {

    // Structures necessaires pour la librairie
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile; // Fichier d'entrée
    unsigned char *ligne;
    Image *image = (Image *)malloc(sizeof(Image)); // Structure de retour

    // Configuration du gestionnaire d'erreurs
    cinfo.err = jpeg_std_error(&jerr);

    // Initialisation du décompresseur JPEG
    jpeg_create_decompress(&cinfo);

    // Ouverture du fichier
    if ((infile = fopen(filename, "rb")) == NULL) {
        printf("Erreur lors de l'ouverture du fichier %s\n", filename);
        return NULL;
    }

    // Decompression
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // Stockage des dimensions de l'image
    image->sizeX = cinfo.output_width;
    image->sizeY = cinfo.output_height;

    // Allocation de la mémoire pour les données de l'image
    image->data = (char *)malloc(3 * image->sizeX * image->sizeY);
    ligne = (unsigned char *)malloc(3 * image->sizeX);

    // Lecture de l'image ligne par ligne
    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *ligneDest = (unsigned char *)image->data +
            3 * image->sizeX * cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, &ligne, 1);
        memcpy(ligneDest, ligne, 3 * image->sizeX);
    }

    // Finalisation de la décompression
    jpeg_finish_decompress(&cinfo);

    // Nettoyage des ressources
    jpeg_destroy_decompress(&cinfo);
    free(ligne);
    fclose(infile);

    return image;
}

// initialisation des textures
void init() {
    // Activation du test de profondeur et configuration de la couleur de fond
    glEnable(GL_DEPTH_TEST);
    glClearColor(fondR, fondG, fondB, 1.0);

    // Configuration de l'éclairage
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  // Lumière directionnelle activée par défaut
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Chargement et initialisation de la texture du corps
    image = loadJPEG("ecailles.jpg");
    if (image != NULL && image->data != NULL) {
        // Génération et configuration de la texture du corps
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Paramètres de la texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Chargement des données de la texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->sizeX, image->sizeY, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, image->data);

        // Vérification des erreurs
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("Erreur OpenGL lors du chargement de la texture du corps: %d\n", err);
        }
    } else {
        printf("Erreur lors du chargement de la texture du corps\n");
    }

    // Chargement et initialisation de la texture de l'oeil
    Image* oeilTexture = loadJPEG("oeil.jpg");
    if (oeilTexture != NULL && oeilTexture->data != NULL) {
        // Génération et configuration de la texture de l'oeil
        glGenTextures(1, &oeilTextureID);
        glBindTexture(GL_TEXTURE_2D, oeilTextureID);

        // Paramètres de la texture de l'oeil
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Chargement des données de la texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, oeilTexture->sizeX, oeilTexture->sizeY, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, oeilTexture->data);

        // Libération de la mémoire
        free(oeilTexture->data);
        free(oeilTexture);

        // Vérification des erreurs
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            printf("Erreur OpenGL lors du chargement de la texture de l'oeil: %d\n", err);
        }
    } else {
        printf("Erreur lors du chargement de la texture de l'oeil\n");
    }
}

// Dessin des faces du corps du poisson avec leur texture plaquée sur celles-ci
void dessinerCorpsPoisson() {

    // Vérifier si la texture est valide
    if (image && image->data) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0, 1.0, 1.0); // Couleur blanche pour ne pas influencer la texture
    }

    // Dessin des faces
    glBegin(GL_QUADS);
    for(int i = 0; i < 44; i++) { // Parcours des faces
        for(int j = 0; j < 4; j++) { // Chaque face a 4 sommets
            int vertexIndex = fPoissonCorps[i][j] - 1;
            if(vertexIndex >= 0 && vertexIndex < 141) {
                if (image && image->data) {
                    // Calculer les coordonnées de texture
                    float u = 0.0f, v = 0.0f;
                    switch(j) {
                        case 0: u = 0.0f; v = 0.0f; break;
                        case 1: u = 1.0f; v = 0.0f; break;
                        case 2: u = 1.0f; v = 1.0f; break;
                        case 3: u = 0.0f; v = 1.0f; break;
                    }
                    glTexCoord2f(u, v);
                }
                // Positionnement des points
                glVertex3f(pPoisson[vertexIndex].x,
                          pPoisson[vertexIndex].y,
                          pPoisson[vertexIndex].z);
            }
        }
    }
    glEnd();

    if (image && image->data) {
        glDisable(GL_TEXTURE_2D);
    }
}

// Animation enclenchée par la touche "a" (nageoires latérales) et automatique (nageoire arrière)
void animer(int value) {
    // Animation des nageoires latérales
    if (animationActive) {
        if (sensAnimation) {
            angleNageoires += 2.5f; // angle augmente de 2.5 degrés
            if (angleNageoires >= 15.0f) sensAnimation = false;
        } else {
            angleNageoires -= 2.5f; // angle réduit de 2.5 degrés
            if (angleNageoires <= -37.5f) sensAnimation = true;
        }
    }

    // Animation de la queue (toujours active)
    angleQueue = 30.0f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.003f);

    glutPostRedisplay();
    glutTimerFunc(35, animer, 0);
}

// Primitive à partir de sa représentation paramétrique : dessin des yeux et application de la texture enroulée
void dessinerSphere(int NM, int NP, float r, bool useTexture, GLuint texture) {
    float coord[NP*NM][3];        // Coordonnées des sommets
    float normal[NP*NM][3];       // Normales des sommets
    float texCoord[NP*NM][2];     // Coordonnées de texture

    // Définition des coordonnées, normales et coordonnées de texture
    for(int j = 0; j < NP; j++) {
        for(int i = 0; i < NM; i++) {
            float theta = (i * 2.0f * PI) / NM;
            float phi = (j * PI) / (NP-1.0f) - (PI/2.0f);

            // Coordonnées de la sphère
            coord[i+j*NM][0] = r * cos(phi) * cos(theta);
            coord[i+j*NM][1] = r * cos(phi) * sin(theta);
            coord[i+j*NM][2] = r * sin(phi);

            // Normales
            normal[i+j*NM][0] = cos(phi) * cos(theta);
            normal[i+j*NM][1] = cos(phi) * sin(theta);
            normal[i+j*NM][2] = sin(phi);

            // Coordonnées de texture
            texCoord[i+j*NM][0] = (float)i / NM;
            texCoord[i+j*NM][1] = (float)j / (NP-1);
        }
    }

    float (*faces)[4] = new float[(NP-1)*NM][4];
    float (*texFaces)[4] = new float[(NP-1)*NM][4];

    // Définition des faces
    for(int j = 0; j < (NP-1); j++) {
        for(int i = 0; i < NM; i++) {
            faces[i+j*NM][0] = i + j * NM;
            faces[i+j*NM][1] = ((i+1) % NM) + j * NM;
            faces[i+j*NM][2] = ((i+1) % NM) + (j + 1) * NM;
            faces[i+j*NM][3] = i + (j + 1) * NM;

            texFaces[i+j*NM][0] = i + j * NM;
            texFaces[i+j*NM][1] = ((i+1) % NM) + j * NM;
            texFaces[i+j*NM][2] = ((i+1) % NM) + (j + 1) * NM;
            texFaces[i+j*NM][3] = i + (j + 1) * NM;
        }
    }

    // Activation de la texture
    if (useTexture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    glEnable(GL_NORMALIZE);

    // Tracé de la sphère face par face
    for(int j = 0; j < (NP-1)*(NM); j++) {
        glBegin(GL_POLYGON);

        // Pour chaque face on dessine ses 4 sommets
        for(int k = 0; k < 4; k++) {
            if (useTexture) {

                // Appliquer les coordonnées de texture pour ce sommet
                int texIndex = texFaces[j][k];
                glTexCoord2f(texCoord[texIndex][0], texCoord[texIndex][1]);
            }

            // Définition de la normale et du sommet pour chaque point de la face
            int vertexIndex = faces[j][k];
            glNormal3f(normal[vertexIndex][0], normal[vertexIndex][1], normal[vertexIndex][2]);
            glVertex3f(coord[vertexIndex][0], coord[vertexIndex][1], coord[vertexIndex][2]);
        }
        glEnd();
    }

    // Désactivation de la texture si elle était utilisée
    if (useTexture) {
        glDisable(GL_TEXTURE_2D);
    }
    glDisable(GL_NORMALIZE);

    // Libération de la mémoire allouée dynamiquement
    delete[] faces;
    delete[] texFaces;
}

void affichage() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Position de la caméra
    glTranslatef(0.0, 0.0, zoom);
    glRotatef(angley, 1.0, 0.0, 0.0);
    glRotatef(anglex, 0.0, 1.0, 0.0);
    glScalef(1.5, 1.5, 1.5);

    // Configuration lumière directionnelle
    GLfloat light0_position[] = { 0.0, 1.0, 1.0, 0.0 };  // w=0 pour lumière directionnelle
    GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };   // Lumière blanche
    GLfloat light0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);

    // Configuration spot (comme une lampe)
    GLfloat light1_position[] = { 0.0, 3.0, 3.0, 1.0 };  // w=1 pour position ponctuelle
    GLfloat light1_diffuse[] = { 1.0, 1.0, 0.7, 1.0 }; // Lumière légèrement jaune
    GLfloat light1_spot_direction[] = { 0.0, -1.0, 0.0 };
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 180); // Angle du cône d'effet de la lumière
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_spot_direction);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT,2.0);


    // Définir les propriétés du matériau
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);


    // ===== Dessiner le corps principal
    dessinerCorpsPoisson();

    // ===== Dessiner la nageoire supérieure
    glPushMatrix();
    glBegin(GL_QUADS); // Quadrilatères pour les faces
    glColor3f(0.447,0.475,0.514); // Gris foncé, couleur sélectionnée à l'aide d'un color picker
    // Parcours des faces
    for(int i = 0; i < 16; i++) {
        // Pour chaque faces, 4 sommets
        for(int j = 0; j < 4; j++) {
            int vertexIndex = fPoissonNageoireSup[i][j] - 1;
            if(vertexIndex >= 0 && vertexIndex < 141) { // Vérification et positionnement des sommets
                glVertex3f(pPoisson[vertexIndex].x,
                          pPoisson[vertexIndex].y,
                          pPoisson[vertexIndex].z);
            }
        }
    }
    glEnd();
    glPopMatrix();

    // ===== Dessiner la nageoire inférieure
    glPushMatrix();
    glBegin(GL_QUADS); // Quadrilatères pour les faces
    glColor3f(0.447,0.475,0.514); // Gris foncé
    // Parcours des faces
    for(int i = 0; i < 12; i++) {
        // Pour chaque face, 4 sommets
       for(int j = 0; j < 4; j++) {
           int vertexIndex = fPoissonNageoireInf[i][j] - 1;
           if(vertexIndex >= 0 && vertexIndex < 141) { // Vérification et positionnement
               glVertex3f(pPoisson[vertexIndex].x,
                         pPoisson[vertexIndex].y,
                         pPoisson[vertexIndex].z);
           }
       }
    }
    glEnd();
    glPopMatrix();


    // Dessiner le visage
    glPushMatrix();
    glBegin(GL_QUADS); // Quadrilatères
    glColor3f(0.565,0.604,0.639); // Gris clair, choisi à l'aide d'un color picker
    for(int i = 0; i < 16; i++) { // Parcours des faces
        for(int j = 0; j < 4; j++) { // Pour chaque faces, 4 sommets
            int vertexIndex = fPoissonTete[i][j] - 1;
            if(vertexIndex >= 0 && vertexIndex < 141) { // Vérification et positionnement
                glVertex3f(pPoisson[vertexIndex].x,
                          pPoisson[vertexIndex].y,
                          pPoisson[vertexIndex].z);
            }
        }
    }
    glEnd();
    glPopMatrix();

    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // ===== Dessiner l'oeil droit
    glPushMatrix();
    // Positionnement sur le rendu
    glTranslatef(0.25, 0.15, 0.70);
    glRotatef(140.0, 0.0, 1.0, 0.0);
    glScalef(0.06, 0.06, 0.06);
    glColor3f(1.0, 1.0, 1.0); // Blanc
    // Définir les propriétés du matériau pour l'oeil
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    // Sphère avec la texture
    dessinerSphere(20, 20, 1.0, true, oeilTextureID);
    glPopMatrix();

    // ====== Dessiner l'oeil gauche
    glPushMatrix();
    // Positionnement sur le rendu (à l'opposé de l'oeil droit)
    glTranslatef(-0.25, 0.15, 0.70);
    glRotatef(40.0, 0.0, 1.0, 0.0);
    glScalef(0.06, 0.06, 0.06);
    glColor3f(1.0, 1.0, 1.0); // Blanc
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    // Sphère texturée
    dessinerSphere(20, 20, 1.0, true, oeilTextureID);
    glPopMatrix();

    glPopAttrib();

    // ===== Dessiner la queue
    glPushMatrix();
    // Positionnement et rotation d'animation
    glTranslatef(0.0, 0.0, -1.92);
    glRotatef(angleQueue, 0.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, 1.92);
    glBegin(GL_QUADS); // Quadrilatères
    glColor3f(0.447,0.475,0.514); // Gris foncé
    for(int i = 0; i < 22; i++) { // Parcours des faces
        for(int j = 0; j < 4; j++) { // Une face, 4 sommets
            int vertexIndex = fPoissonArriere[i][j] - 1;
            if(vertexIndex >= 0 && vertexIndex < 141) { // Vérification et positionnement
                glVertex3f(pPoisson[vertexIndex].x,
                          pPoisson[vertexIndex].y,
                          pPoisson[vertexIndex].z);
            }
        }
    }
    glEnd();
    glPopMatrix();

    // ===== Dessiner la nageoire gauche
    glPushMatrix();
    // Positionnement et rotation d'animation
    glTranslatef(0.47, 0.0, 0.0);
    glRotatef(angleNageoires, 0.0, 1.0, 0.0);
    glTranslatef(-0.47, 0.0, 0.0);
    glBegin(GL_QUADS); // Quadrilatères
    glColor3f(0.447,0.475,0.514); // Gris foncé
    for(int i = 0; i < 10; i++) { // Parcours des faces
        for(int j = 0; j < 4; j++) { // Chaque face, 4 sommets
            int vertexIndex = fPoissonGauche[i][j] - 1;
            if(vertexIndex >= 0 && vertexIndex < 141) { // Vérification et positionnement
                glVertex3f(pPoisson[vertexIndex].x,
                          pPoisson[vertexIndex].y,
                          pPoisson[vertexIndex].z);
            }
        }
    }
    glEnd();
    glPopMatrix();

    // Dessiner la nageoire droite
    glPushMatrix();
    // Positionnement et rotation d'animation
    glTranslatef(-0.47, 0.0, 0.0);
    glRotatef(-angleNageoires, 0.0, 1.0, 0.0);
    glTranslatef(0.47, 0.0, 0.0);
    glBegin(GL_QUADS); // Quadrilatères
    glColor3f(0.447,0.475,0.514); // Gris foncé
    for(int i = 0; i < 10; i++) { // Parcours des faces
        for(int j = 0; j < 4; j++) { // Chaque face, 4 sommets
            int vertexIndex = fPoissonDroite[i][j] - 1;
            if(vertexIndex >= 0 && vertexIndex < 141) { // Vérification et positionnement
                glVertex3f(pPoisson[vertexIndex].x,
                          pPoisson[vertexIndex].y,
                          pPoisson[vertexIndex].z);
            }
        }
    }
    glEnd();
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);

    // Activer le double buffering et le tampon de profondeur
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // Obtenir les dimensions de l'écran
    int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    // Définir une taille de fenêtre initiale proportionnelle à l'écran
    int windowWidth = screenWidth * 0.8;  // 80% de la largeur de l'écran
    int windowHeight = screenHeight * 0.8; // 80% de la hauteur de l'écran

    // Centrer la fenêtre sur l'écran
    int windowPosX = (screenWidth - windowWidth) / 2;
    int windowPosY = (screenHeight - windowHeight) / 2;

    // Configurer et créer la fenêtre
    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Poisson 3D");

    // Initialisation
    glEnable(GL_DEPTH_TEST);

    // Lumière
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);  // Lumière directionnelle
    // glEnable(GL_LIGHT1);  // Lumière spot, eteinte par defaut
    glEnable(GL_COLOR_MATERIAL);
    init();

    // Vérifier si l'initialisation de la texture a réussi
    if (!image || !image->data) {
        printf("Attention: La texture n'a pas pu être chargée.\n");
    }

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


    // Enregistrer les fonctions de callback
    glutDisplayFunc(affichage);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(clavier);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);


    // Démarrer le timer une seule fois au démarrage pour l'animation
    if (!timerStarted) {
        timerStarted = true;
        glutTimerFunc(50, animer, 0);
    }

    // Démarrer la boucle principale
    glutMainLoop();
    return 0;
}

// Fonction d'aide pour mettre à jour la couleur du fond selon l'état des lumières
void majLumiereFond() {
    if (light0On && light1On) {
        // Les deux lumières sont allumées : fond plus lumineux
        glClearColor(fondR * fondBrightFactor,
                    fondG * fondBrightFactor,
                    fondB * fondBrightFactor,
                    1.0);
    } else if (light0On || light1On) {
        // Une seule lumière est allumée : luminosité normale
        glClearColor(fondR * fondNormalFactor,
                    fondG * fondNormalFactor,
                    fondB * fondNormalFactor,
                    1.0);
    } else {
        // Aucune lumière n'est allumée : fond sombre
        glClearColor(fondR * fondDarkFactor,
                    fondG * fondDarkFactor,
                    fondB * fondDarkFactor,
                    1.0);
    }
}

// Gestion des actions enclenchée par des touches du clavier
void clavier(unsigned char touche, int x, int y) {
    switch (touche) {
        case 'p': // Affichage du poisson plein
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glutPostRedisplay();
            break;
        case 'f': // Affichage en mode fil de fer
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glutPostRedisplay();
            break;
        case 's': // Affichage en mode sommets seuls
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glutPostRedisplay();
            break;
        case 'z': // Zoomer
            zoom += 0.5;
            glutPostRedisplay();
            break;
        case 'Z': // Dézoomer
            zoom -= 0.5;
            glutPostRedisplay();
            break;
        case 'a': // Lancer l'animation des nageoires latérales
            animationActive = !animationActive;
            glutPostRedisplay();
            break;
        case 'l': // Allumer la lumière directionnelle
            glEnable(GL_LIGHT0);
            light0On = true;
            majLumiereFond();
            glutPostRedisplay();
            break;
        case 'L': // Eteindre la lumière directionnelle
            glDisable(GL_LIGHT0);
            light0On = false;
            majLumiereFond();
            glutPostRedisplay();
            break;
        case 'm': // Allumer la lumière spot
            glEnable(GL_LIGHT1);
            light1On = true;
            majLumiereFond();
            glutPostRedisplay();
            break;
        case 'M': // Eteindre la lumière spot
            glDisable(GL_LIGHT1);
            light1On = false;
            majLumiereFond();
            glutPostRedisplay();
            break;
            break;
        case 'q': // Quitter le programme
            exit(0);
    }
}

// Gestion de la caméra avec les flèches directionnelles
void special(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:  // Flèche de gauche
            anglex -= 5;
            break;
        case GLUT_KEY_RIGHT: // Flèche de droite
            anglex += 5;
            break;
        case GLUT_KEY_UP:    // Flèche du haut
            angley -= 5;
            break;
        case GLUT_KEY_DOWN:  // Flèche du bas
            angley += 5;
            break;
    }
    glutPostRedisplay();
}

void reshape(int width, int height) {
    // Empêcher une division par zéro dans la suite de la fonction
    if (height == 0)
        height = 1;

    // Calculer le ratio de l'aspect
    float aspect = (float)width / (float)height;

    // Définir le viewport pour occuper toute la fenêtre
    glViewport(0, 0, width, height);

    // Réinitialiser la matrice de projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Définir la perspective en tenant compte du ratio d'aspect
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);

    // Revenir à la matrice modelview
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse(int button, int state, int x, int y) {
    // si on appuie sur le bouton gauche
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        presse = 1; // le booleen presse passe a 1 (vrai)
        xold = x;   // on sauvegarde la position de la souris
        yold = y;
    }
    // si on relache le bouton gauche
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
        presse = 0; // le booleen presse passe a 0 (faux)
}

void mousemotion(int x, int y) {
    if (presse) // si le bouton gauche est pressé
    {
        /* on modifie les angles de rotation de l'objet
           en fonction de la position actuelle de la souris et de la derniere
           position sauvegardee */
        anglex = anglex + (x - xold);
        angley = angley + (y - yold);
        glutPostRedisplay(); // on demande un rafraichissement de l'affichage
    }

    xold = x; // sauvegarde des valeurs courantes de la position de la souris */
    yold = y;
}
