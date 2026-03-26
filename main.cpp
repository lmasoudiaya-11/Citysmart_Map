#include "raylib.h"
#include "raymath.h"
#include <vector>


// =====================================================
// ENUMS
// =====================================================

enum TileType {
    GRASS = 0,
    ROAD_H,
    ROAD_V,
    PROAD_V,
    PROAD_V1,
    INTERSECTION,
    ROUNDABOUT,
    CURVERTOP,
    CURVERTOP1,
    CURVERBOTTOM,
    FCURVERBOTTOM,
    CURVERBOTTOM1,
    TROAD,
    TROAD1,
    ROTROAD,
    PARKING,
    PPARKING1,
    PPARKING1h , 
    PPARKING2,
    PPARKING2h,
    PLAYGROUND3 
    
};

enum ZoneType {
    NONE = 0,
    DOWNTOWN,
    EMERGENCY,
    MEDICAL,
    EDUCATION,
};

enum FacilityType {
    NO = 0,
    RESTAURANT,
    SHOP ,
    BCOFFEE,
    HOSPITAL,
    HOSPITAL1 , 
    HOSPITAL2 ,
    HOSPITAL3 ,
    PHARMACY , 
    BUILDING_b1 , 
    FIRES , 
    FIRES1 , 
    FIRES2 , 
    COFFE_S , 
    BAKERY ,
    OFFICE_C , 
    MINIMARKET , 
    POLICES , 
    POLICES1 , 
    POLICES2 , 
    SCHOOL ,
    SCHOOL1 , 
    SCHOOL2 , 
    GSC ,
    BOOKSTORE , 
    BUILDING_b71,
    BUILDING_b72,
    BUILDING_b8,
    BUILDING_b9 ,
    BOUTIQUE

};

enum FacingDirection
{
    FACE_N, // -Z (north)
    FACE_S, // +Z (south)
    FACE_E,  // +X (east)
    FACE_W   // -X (west)
};

enum TrafficLightState {
    TL_RED,
    TL_ORANGE,
    TL_GREEN
};

// =====================================================
// MAP MACROS
// =====================================================

#define G  GRASS
#define H  ROAD_H
#define V  ROAD_V
#define PV PROAD_V
#define PV1 PROAD_V1
#define I  INTERSECTION
#define R  ROUNDABOUT
#define CT CURVERTOP
#define CT1 CURVERTOP1
#define CB CURVERBOTTOM
#define FCB FCURVERBOTTOM
#define CB1 CURVERBOTTOM1
#define TR TROAD
#define TR1 TROAD1
#define RTT ROTROAD
#define P  PARKING
#define PP1 PPARKING1
#define PP1H PPARKING1h
#define PP2 PPARKING2
#define PP2H PPARKING2h
#define PG3 PLAYGROUND3


// =====================================================
// TILE MAP
// =====================================================

static int TILE_MAP[10][10] = {
    {PP2,PG3,G,G,G,G,V,G,G,G},
    {PP1,G,CT,H,CB1,PP2,V,G,CT,PV1},

    {PV,H,RTT,G,TR,PV,I,H,CB,PP1H},
    {G,G,V,G,V,G,V,G,G,PP2H},

    {G,G,V,G,V,G,V,G,G,G},
    {H,H,TR1,H,R,H,I,H,CB1,G},

    {G,G,G,G,V,G,V,G,V,G},
    {CT,H,H,H,I,H,CB,G,CT1,H},

    {V,G,G,G,V,PP2,G,G,G,PP2},
    {V,G,G,G,TR,PV,H,PV,PV,PV}
};

// =====================================================
// FACILITY MAP
// =====================================================

#define N NO
#define REST RESTAURANT
#define HP HOSPITAL
#define HP1 HOSPITAL1
#define HP2 HOSPITAL2
#define HP3 HOSPITAL3
#define PM PHARMACY
#define B1 BUILDING_b1
#define F1 FIRES
#define F2 FIRES1 
#define F3 FIRES2
#define BC BCOFFEE
#define CS COFFE_S
#define BKE BAKERY
#define OC OFFICE_C
#define MM MINIMARKET
#define PS POLICES
#define PS1 POLICES1
#define PS2 POLICES2
#define S SCHOOL
#define S1 SCHOOL1
#define S2 SCHOOL2
#define GSC GSC
#define BKS BOOKSTORE
#define B71 BUILDING_b71
#define B72 BUILDING_b72
#define B8  BUILDING_b8
#define B9  BUILDING_b9
#define BQ BOUTIQUE

static FacilityType FACILITY_MAP[10][10] = {

    {N,N,S,S1,S2,BKS,N,N,MM,BKE},
    {N,GSC,N,N,N,N,N,N,N,N},

    {N,N,N,N,N,N,N,N,N,N},
    {N,PS1,N,N,N,B8,N,N,N,N},

    {PS2,PS,N,BQ,N,B9,N,PM,HP3,HP2},
    {N,N,N,N,N,N,N,N,N,HP1},

    {REST,N,BC,OC,N,CS,N,N,N,HP},
    {N,N,N,N,N,N,N,N,N,N},

    {N,B1,N,B1,N,N,F2,F1,F3,N},
    {N,B71,N,B72,N,N,N,N,N,N}
};

// =====================================================
// DRAW HELPERS
// =====================================================


void DrawTile(Model &model, Texture2D texture, Vector3 pos,float width, float height, float depth, float rotationY ,bool isNight)
{
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Matrix scale = MatrixScale(width, height, depth);
    Matrix rot   = MatrixRotateY(rotationY * DEG2RAD);
    Matrix trans = MatrixTranslate(pos.x, pos.y, pos.z);

    model.transform = MatrixMultiply(scale, MatrixMultiply(rot, trans));
          Color tint = isNight ? DARKGRAY : WHITE;

    DrawModel(model, Vector3Zero(), 1.0f, WHITE);
    model.transform = MatrixIdentity();
}

struct Building {
    FacilityType type;
    Vector3 pos;
    float width;
    float height;
    float depth;

    FacingDirection frontDir;
    FacingDirection backDir;  
    FacingDirection side1Dir;
    FacingDirection side2Dir;

};

void DrawBoxBuilding(Model& cube, const Building& b,
                     Texture2D frontTex,
                     Texture2D side1Tex,
                     Texture2D side2Tex,
                     Texture2D backTex,
                     Texture2D roofTex,
                     bool isNight)
{
    float wallThickness = 0.2f;   // wall thickness
    float offset = 0.01f;         // small gap to avoid Z-fighting
    float halfW = b.width / 2.0f;
    float halfD = b.depth / 2.0f;
    float wallOffset = wallThickness / 2.0f;

    auto drawWall = [&](FacingDirection dir, float lengthX, float lengthZ, Texture2D tex,  Vector3 pos)
    {
        cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = tex;
        cube.transform = MatrixMultiply(
            MatrixScale(lengthX, b.height, lengthZ),
            MatrixTranslate(pos.x, pos.y + b.height / 2 + offset, pos.z)
        );
        DrawModel(cube, Vector3Zero(), 1.0f, WHITE);
    };

    // --- FRONT WALL ---
    if (b.frontDir == FACE_N || b.frontDir == FACE_S)
        drawWall(b.frontDir, b.width, wallThickness, frontTex,
                 {b.pos.x, b.pos.y, b.pos.z + ((b.frontDir == FACE_N) ? -halfD + wallOffset : halfD - wallOffset)});
    else
        drawWall(b.frontDir, wallThickness, b.depth, frontTex,
                 {b.pos.x + ((b.frontDir == FACE_E) ? halfW - wallOffset : -halfW + wallOffset), b.pos.y, b.pos.z});

    // --- BACK WALL ---
    if (b.backDir == FACE_N || b.backDir == FACE_S)
        drawWall(b.backDir, b.width, wallThickness, backTex,
                 {b.pos.x, b.pos.y, b.pos.z + ((b.backDir == FACE_N) ? -halfD + wallOffset : halfD - wallOffset)});
    else
        drawWall(b.backDir, wallThickness, b.depth, backTex,
                 {b.pos.x + ((b.backDir == FACE_E) ? halfW - wallOffset : -halfW + wallOffset), b.pos.y, b.pos.z});

    // --- SIDE1 WALL ---
    if (b.side1Dir == FACE_N || b.side1Dir == FACE_S)
        drawWall(b.side1Dir, b.width, wallThickness, side1Tex,  
                 {b.pos.x, b.pos.y, b.pos.z + ((b.side1Dir == FACE_N) ? -halfD + wallOffset : halfD - wallOffset)});
    else
        drawWall(b.side1Dir, wallThickness, b.depth, side1Tex,
                 {b.pos.x + ((b.side1Dir == FACE_E) ? halfW - wallOffset : -halfW + wallOffset), b.pos.y, b.pos.z});

    // --- SIDE2 WALL ---
    if (b.side2Dir == FACE_N || b.side2Dir == FACE_S)
        drawWall(b.side2Dir, b.width, wallThickness, side2Tex,
                 {b.pos.x, b.pos.y, b.pos.z + ((b.side2Dir == FACE_N) ? -halfD + wallOffset : halfD - wallOffset)});
    else
        drawWall(b.side2Dir, wallThickness, b.depth, side2Tex,
                 {b.pos.x + ((b.side2Dir == FACE_E) ? halfW - wallOffset : -halfW + wallOffset), b.pos.y, b.pos.z});

// --- ROOF ---
cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = roofTex;

// Slightly larger than walls but much smaller than previous overhang
float roofWidth  = b.width  + wallThickness / 4.0f;  // small extra overhang
float roofDepth  = b.depth  + wallThickness / 4.0f;
float roofHeight = wallThickness; // thickness of the roof

cube.transform = MatrixMultiply(
    MatrixScale(roofWidth, roofHeight, roofDepth),
    MatrixTranslate(b.pos.x, b.pos.y + b.height + roofHeight / 2 + offset, b.pos.z)
);

DrawModel(cube, Vector3Zero(), 1.0f, WHITE);

DrawModel(cube, Vector3Zero(), 1.0f, WHITE);
}

void DrawTallBuilding(
    Model& cube,
    const Building& b,
    int floors,
    float floorHeight,
    

    // base (shops / doors)
    Texture2D frontBase,
    Texture2D side1Base,
    Texture2D side2base ,
    Texture2D backBase,

    // middle (windows)
    Texture2D frontMid,
    Texture2D side1Mid,
    Texture2D side2Mid,
    Texture2D backMid,

    // roof
    Texture2D roofTex,
    bool isNight
)

{
    Building part = b;
    part.height = floorHeight;

    // ---- BASE FLOOR ----
    part.pos.y = b.pos.y;
    DrawBoxBuilding(cube, part,
        frontBase,
        side1Base,
        side2base , 
        backBase,
        roofTex,
        isNight
      
        
    );

    // ---- UPPER FLOORS ----
    for (int i = 1; i < floors; i++)
    {
        part.pos.y = b.pos.y + i * floorHeight;

        DrawBoxBuilding(cube, part,
            frontMid,
            side1Mid,
            side2Mid,
            backMid,
            roofTex,
            isNight
        );
    }
}


void DrawWindowGlow(const Building& b, float height)
{
    Color glow = Color{255, 220, 120, 80}; // warm light, transparent

    Vector3 size = {
        b.width * 0.98f,
        height * 0.9f,
        b.depth * 0.98f
    };

    Vector3 pos = {
        b.pos.x,
        b.pos.y + height / 2.0f,
        b.pos.z
    };

    DrawCubeV(pos, size, glow);
}

struct TrafficLight {
    Vector3 pos;
    TrafficLightState state;
    float timer;
};

void DrawTrafficLightImage(
    Model& cube,
    const TrafficLight& tl,
    Texture2D redTex,
    Texture2D orangeTex,
    Texture2D greenTex
)
{
    Texture2D currentTex;

    switch (tl.state)
    {
        case TL_RED:    currentTex = redTex;    break;
        case TL_ORANGE: currentTex = orangeTex; break;
        case TL_GREEN:  currentTex = greenTex;  break;
    }

    cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = currentTex;

    float width  = 0.8f;
    float height = 2.0f;
    float depth  = 0.05f; // very thin (image look)

    cube.transform = MatrixMultiply(
        MatrixScale(width, height, depth),
        MatrixTranslate(
            tl.pos.x,
            tl.pos.y + height / 2.0f,
            tl.pos.z
        )
    );

    DrawModel(cube, Vector3Zero(), 1.0f, WHITE);
    cube.transform = MatrixIdentity();
}

// =====================================================
// MAIN
// =====================================================

int main()
{
    InitWindow(1920, 1080, "Smart City - Emergency Demo");
    
    SetTargetFPS(60);

    bool isNight = false;

    const float TILE = 4.0f;

    Camera3D cam = {0};
    cam.position = {40, 35, 40};
    cam.target   = {20, 0, 20};
    cam.up       = {0, 1, 0};
    cam.fovy     = 60;
    cam.projection = CAMERA_PERSPECTIVE;

    Vector3 camStartPos = cam.position;
    Vector3 camStartTarget = cam.target;
    float camStartFovy = cam.fovy;
    float camAngle = 45.0f;
    float camRadius = 55.0f;
    float camHeight = 30.0f;

   
    // ---------------- TEXTURES ----------------

    // Roads_Elements

    Texture2D g   = LoadTexture("grass.png");
    Texture2D RH  = LoadTexture("road_h.png");
    Texture2D RV  = LoadTexture("road_v.png");
    Texture2D PR  = LoadTexture("proad_v.png");
    Texture2D PR1  = LoadTexture("proad_v1.png");
    Texture2D INT = LoadTexture("intersection.png");
    Texture2D CVT = LoadTexture("cvroad_t.png");
    Texture2D CVT1= LoadTexture("cvroad_t1.png");
    Texture2D CVB = LoadTexture("cvroad_b.png");
    Texture2D FCVB = LoadTexture("f2cvb.png");
    Texture2D CVB1= LoadTexture("cvroad_b1.png");
    Texture2D TRD = LoadTexture("t_road.png");
    Texture2D TRD1= LoadTexture("t_road1.png");
    Texture2D ROT = LoadTexture("rott_road.png");
    Texture2D PK  = LoadTexture("parking.png");
    Texture2D PK1 = LoadTexture("pparking_1.png");
     Texture2D PK1h = LoadTexture("pparking_1h.png");
    Texture2D PK2 = LoadTexture("pparking_2.png");
     Texture2D PK2h = LoadTexture("pparking_2h.png");
    Texture2D RB = LoadTexture("roundabout.png");
    Texture2D PY3 = LoadTexture("playground_3.png");

    //Buildings

    //Restaurant
    Texture2D FREST = LoadTexture("f_restaurant.png");
    Texture2D RBREST = LoadTexture("rb_restaurant.png");

    //BlackCoffe_Shop
    Texture2D FBCOF = LoadTexture("f_bcoffe.png");
    Texture2D RBBCOF = LoadTexture("rb_bcoffe.png");

    //Hospital & Pharmacy
    Texture2D FHT = LoadTexture("f_hospital.png");
    Texture2D HT = LoadTexture("f1_hospital.png");
    Texture2D HT1 = LoadTexture("f2_hospital.png");
    Texture2D HTS1 = LoadTexture("s1_hospital.png");
    Texture2D HTS2 = LoadTexture("s2_hospital.png");
    Texture2D PHM = LoadTexture("f_pharmacy.png");
    Texture2D PHMR = LoadTexture("r_pharmacy.png");


    Texture2D BF1 = LoadTexture("b1_f1.png");
    Texture2D BF2 = LoadTexture("b1_f2.png");
    Texture2D BS1 = LoadTexture("b1_s1.png");
    Texture2D BS2 = LoadTexture("b1_s2.png");
    Texture2D BS3 = LoadTexture("b1_s3.png");
    Texture2D BS4 = LoadTexture("b1_s4.png");
    Texture2D BR = LoadTexture("b1_r.png");
    Texture2D B2F = LoadTexture("b2_f.png");
    Texture2D B2F2 = LoadTexture("b2_f2.png");
    Texture2D B2R = LoadTexture("b2_r.png");
    Texture2D B3F = LoadTexture("b3_f.png");
    Texture2D B3R = LoadTexture("b3_r.png");
    Texture2D B4F = LoadTexture("b4_f.png");
    Texture2D B4R = LoadTexture("b4_r.png");
    Texture2D B5F = LoadTexture("b5_f.png");
    Texture2D B5F1 = LoadTexture("b5_f1.png");
    Texture2D B5F2 = LoadTexture("b5_f2.png");
    Texture2D B5S = LoadTexture("b5_s.png");
    Texture2D B5S1 = LoadTexture("b5_s1.png");
    Texture2D B5R = LoadTexture("b5_r.png");
    Texture2D B6F1 = LoadTexture("b6_f1.png");
    Texture2D B6F3 = LoadTexture("b6_f3.png");
    Texture2D B6R = LoadTexture("b6_r.png");
    Texture2D MMF = LoadTexture("minim_f.png");
    Texture2D MMS = LoadTexture("minim_s.png");
    Texture2D B7F = LoadTexture("b7_f.png");
    Texture2D B7F1 = LoadTexture("b7_f1.png");
    Texture2D B7F2 = LoadTexture("b7_f2.png");
    Texture2D B7S = LoadTexture("b7_s.png");
    Texture2D B7S1 = LoadTexture("b7_s1.png");
    Texture2D B7R = LoadTexture("b7_r.png");
    Texture2D B8F = LoadTexture("b8_f.png");
    Texture2D B8F2 = LoadTexture("b8_f2.png");
    Texture2D B8S = LoadTexture("b8_s.png");
    Texture2D B8S2 = LoadTexture("b8_s2.png");
    Texture2D B8R = LoadTexture("b8_r.png");
    Texture2D B9F = LoadTexture("b9_f.png");
    Texture2D B9F1 = LoadTexture("b9_f1.png");
    Texture2D B9S = LoadTexture("b9_s.png");
    Texture2D B9S1 = LoadTexture("b9_s1.png");
    Texture2D B9R = LoadTexture("b9_r.png");

    //Fire_station
    Texture2D FS1 = LoadTexture("f1_fire.png");
    Texture2D FS2 = LoadTexture("f2_fire.png");
    Texture2D FS3 = LoadTexture("f3_fire.png");
    Texture2D FRS = LoadTexture("rs_fire.png");
    Texture2D FS = LoadTexture("s_fire.png");
    Texture2D BF = LoadTexture("b_fire.png");

    //CoffeHouse
    Texture2D FCS = LoadTexture("f_coffes.png");
    Texture2D F1CS = LoadTexture("f1_coffes.png");
    Texture2D CSS = LoadTexture("s_coffes.png");
    Texture2D CS1 = LoadTexture("s1_coffes.png");
    Texture2D RCS = LoadTexture("rs_coffes.png");

    Texture2D BK = LoadTexture("bakery.png");
    Texture2D RBY = LoadTexture("r_bakery.png");

    //Police_station
    Texture2D RP = LoadTexture("r_police.png");
    Texture2D FP = LoadTexture("f_police.png");
    Texture2D SP = LoadTexture("s_police.png");
    Texture2D S1P = LoadTexture("s1_police.png");

    //schoolFacilities
    Texture2D FSL = LoadTexture("f_school.png");
    Texture2D F1SL = LoadTexture("f1_school.png");
    Texture2D FSLS = LoadTexture("s_school.png");
    Texture2D FSLR = LoadTexture("r_school.png");

    Texture2D FGSC = LoadTexture("f_GSC.png");
    Texture2D SGSC = LoadTexture("s_GSC.png");
    Texture2D RGSC = LoadTexture("r_GSC.png");

    Texture2D FBKS = LoadTexture("f_bookstore.png");
    Texture2D SBKS = LoadTexture("s_bookstore.png");
    Texture2D RBKS = LoadTexture("r_bookstore.png");

    //Boutique 
    Texture2D FBTQ = LoadTexture("f_boutique.png");
    Texture2D SBTQ = LoadTexture("s_boutique.png");
    Texture2D RBTQ = LoadTexture("r_boutique.png");

    Texture2D TL_RED_TEX    = LoadTexture("traffic_red.png");
Texture2D TL_ORANGE_TEX = LoadTexture("traffic_orange.png");
Texture2D TL_GREEN_TEX  = LoadTexture("traffic_green.png");


    Model cube = LoadModelFromMesh(GenMeshCube(1,1,1));
    Model wallPlane = LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1));


   // ---------------- BUILD FACILITIES ----------------


std::vector<Building> buildings;

for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 10; x++) {

        FacilityType f = FACILITY_MAP[y][x];
        if (f == NO) continue;
        if (TILE_MAP[y][x] != GRASS) continue; // only place buildings on grass

        Building b;
        b.type = f;
        b.pos  = { x * TILE, 0.0f, y * TILE };

        // default dimensions to cover full tile
        b.width  = TILE;
        b.height = 3.0f;
        b.depth  = TILE;

        // assign frontDir based on facility
        switch(f)
        {
           case RESTAURANT: b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_S;  b.side1Dir = FACE_E;  
           break;

           case BCOFFEE: b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_W; b.side1Dir = FACE_N;  
           break;

           case HOSPITAL: b.width  = TILE; b.depth  = TILE;  b.frontDir = FACE_W;  b.side1Dir = FACE_S;  
           break;

           case HOSPITAL1: b.width  = TILE;  b.depth  = TILE; b.frontDir = FACE_W; b.side1Dir = FACE_N;  
           break;   

           case HOSPITAL2:  b.width  = TILE; b.depth  = TILE ; b.frontDir = FACE_N;  b.side1Dir = FACE_E;  
           break;     

           case HOSPITAL3: b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_S;  b.side1Dir = FACE_W;
           break;  

           case PHARMACY: b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_W; b.side1Dir = FACE_S;  
           break;     
               
            case BUILDING_b1: b.width  = TILE ; b.depth  = TILE; b.frontDir = FACE_N; b.side1Dir = FACE_E;   
            break ;

            case FIRES : b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_N; b.side1Dir = FACE_E; 
            break;

            case FIRES1 : b.width  = TILE; b.depth  = TILE ;  b.frontDir = FACE_N; b.side1Dir = FACE_E;  
            break;

            case FIRES2 : b.width  = TILE;  b.depth  = TILE; b.frontDir = FACE_S; b.side1Dir = FACE_E;  
            break;
       
            case COFFE_S : b.width  = TILE;  b.depth  = TILE; b.frontDir = FACE_N; b.side1Dir = FACE_E; 
            break;

            case BAKERY : b.width  = TILE;  b.depth  = TILE;  b.frontDir = FACE_S; b.side1Dir = FACE_E;  
            break;

            case OFFICE_C :  b.width  = TILE;   b.depth  = TILE; b.frontDir = FACE_N;  b.side1Dir = FACE_W;  
            break;  

            case MINIMARKET :  b.width  = TILE;  b.depth  = TILE;  b.frontDir = FACE_E;  b.side1Dir = FACE_N;  
            break;

            case POLICES:  b.width  = TILE;  b.depth  = TILE;  b.frontDir = FACE_N;  b.side1Dir = FACE_E;  
            break;

            case POLICES1 : b.width  = TILE;  b.depth  = TILE;  b.frontDir = FACE_N; b.side1Dir = FACE_E;  
            break;

            case POLICES2 : b.width  = TILE; b.depth  = TILE ;  b.frontDir = FACE_S; b.side1Dir = FACE_E;  
            break; 

            case SCHOOL : b.width  = TILE;  b.depth  = TILE; b.frontDir = FACE_N;  b.side1Dir = FACE_E;  
            break;

            case SCHOOL1 : b.width  = TILE;  b.depth  = TILE;  b.frontDir = FACE_S;  b.side1Dir = FACE_W;  
            break;

            case SCHOOL2 : b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_W;  b.side1Dir = FACE_N;  
            break;

            case GSC : b.width  = TILE ;  b.depth  = TILE; b.frontDir = FACE_S; b.side1Dir = FACE_E;  
            break ;

            case BOOKSTORE :  b.width  = TILE;  b.depth  = TILE; b.frontDir = FACE_N;  b.side1Dir = FACE_E;  
            break ;   

            case BUILDING_b71 : b.width  = TILE;  b.depth  = TILE ;  b.frontDir = FACE_N; b.side1Dir = FACE_E;  
            break ;

            case BUILDING_b72 : b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_N;  b.side1Dir = FACE_E;  
            break ;   

            case BUILDING_b8 :  b.width  = TILE;  b.depth  = TILE; b.frontDir = FACE_E;  b.side1Dir = FACE_N;  
            break ;
                
            case BUILDING_b9 : b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_E; b.side1Dir = FACE_S;  
            break ;   

            case BOUTIQUE : b.width  = TILE; b.depth  = TILE; b.frontDir = FACE_E; b.side1Dir = FACE_S;  
            break ; 
        
            default: b.frontDir = FACE_S; b.side1Dir = FACE_E;
            break;
}

        // backDir and side2Dir will be automatically calculated
        b.backDir = (b.frontDir == FACE_N) ? FACE_S :
                    (b.frontDir == FACE_S) ? FACE_N :
                    (b.frontDir == FACE_E) ? FACE_W :
                    FACE_E;

        b.side2Dir = (b.side1Dir == FACE_N) ? FACE_S :
             (b.side1Dir == FACE_S) ? FACE_N :
             (b.side1Dir == FACE_E) ? FACE_W :
             FACE_E;            

        buildings.push_back(b);
    }
}


std::vector<TrafficLight> trafficLights;

for (int y = 0; y < 10; y++)
{
    for (int x = 0; x < 10; x++)
    {
        if (TILE_MAP[y][x] == INTERSECTION)
        {
            TrafficLight tl;
            tl.pos = {
                x * TILE + TILE * 0.45f,
                0.0f,
                y * TILE + TILE * 0.45f
            };
            tl.state = TL_RED;
            tl.timer = 0.0f;

            trafficLights.push_back(tl);
        }
    }
}



    // ---------------- MAIN LOOP ---------------------
    while (!WindowShouldClose())
    {   float moveSpeed = 20.0f * GetFrameTime();
 
  // Forward / Backward (W / S)
  if (IsKeyDown(KEY_W)) { cam.position.z -= moveSpeed; cam.target.z   -= moveSpeed; }
  if (IsKeyDown(KEY_S)) { cam.position.z += moveSpeed; cam.target.z   += moveSpeed; }

  // Left / Right (A / D)
  if (IsKeyDown(KEY_A)) { cam.position.x -= moveSpeed; cam.target.x   -= moveSpeed; }
  if (IsKeyDown(KEY_D)) { cam.position.x += moveSpeed; cam.target.x   += moveSpeed; }

  // Zoom IN (Z)
  if (IsKeyDown(KEY_Z)) { cam.fovy -= 40 * GetFrameTime(); if (cam.fovy < 20) cam.fovy = 20; }

  // Zoom OUT (X)
  if (IsKeyDown(KEY_X)) { cam.fovy += 40 * GetFrameTime(); if (cam.fovy > 90) cam.fovy = 90; }

  // Reset camera (R)
  if (IsKeyPressed(KEY_R)) { cam.position = camStartPos; cam.target = camStartTarget; cam.fovy = camStartFovy; }

  // Rotate
  if (IsKeyDown(KEY_LEFT))  camAngle -= 60 * GetFrameTime();
  if (IsKeyDown(KEY_RIGHT)) camAngle += 60 * GetFrameTime();

  // Zoom
  if (IsKeyDown(KEY_UP))   camRadius -= 20 * GetFrameTime();
  if (IsKeyDown(KEY_DOWN)) camRadius += 20 * GetFrameTime();
    
  // Clamp zoom
  camRadius = Clamp(camRadius, 15.0f, 120.0f);

  // Update ORBIT camera position
  cam.position.x = cam.target.x + cosf(camAngle * DEG2RAD) * camRadius;
  cam.position.z = cam.target.z + sinf(camAngle * DEG2RAD) * camRadius;
  cam.position.y = camHeight;

  // NightMode(isn't workinng)
  if (IsKeyPressed(KEY_J)) isNight = !isNight;



        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(cam);

    // ---------------------- MAP -------------------------
        // MAP DRAWING
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {

                Vector3 pos = { x * TILE, 0, y * TILE };

                if (TILE_MAP[y][x] == GRASS)
                    DrawTile(cube, g, pos, TILE, 0.1f, TILE, 0 , isNight);

                switch (TILE_MAP[y][x]) {
                    case ROAD_H: DrawTile(cube, RH, pos, TILE, 0.11f, TILE, 0, isNight); break;
                    case ROAD_V: DrawTile(cube, RV, pos, TILE, 0.11f, TILE, 0, isNight); break;
                    case PROAD_V:DrawTile(cube, PR, pos, TILE, 0.11f, TILE, 90, isNight); break;
                    case PROAD_V1:DrawTile(cube, PR, pos, TILE, 0.11f, TILE, -90, isNight); break;
                    case INTERSECTION: DrawTile(cube, INT, pos, TILE, 0.11f, TILE, 0, isNight); break;
                    case ROUNDABOUT: DrawTile(cube, RB, pos, TILE, 0.11f, TILE, 0 , isNight); break;
                    case CURVERTOP: DrawTile(cube, CVT, pos, TILE, 0.11f, TILE, 0 , isNight); break;
                    case CURVERTOP1:DrawTile(cube, CVT1,pos,TILE,0.11f,TILE,0, isNight); break;
                    case CURVERBOTTOM:DrawTile(cube,CVB,pos,TILE,0.11f,TILE,180, isNight); break;
                    case FCURVERBOTTOM:DrawTile(cube,FCVB,pos,TILE,0.11f,TILE,-180, isNight); break;
                    case CURVERBOTTOM1:DrawTile(cube,CVB1,pos,TILE,0.11f,TILE,0, isNight); break;
                    case TROAD:DrawTile(cube,TRD,pos,TILE,0.11f,TILE,0, isNight); break;
                    case TROAD1:DrawTile(cube,TRD1,pos,TILE,0.11f,TILE,180, isNight); break;
                    case ROTROAD:DrawTile(cube,ROT,pos,TILE,0.11f,TILE,0, isNight); break;
                    case PARKING:DrawTile(cube,PK,pos,TILE,0.11f,TILE,90, isNight); break;
                    case PPARKING1:DrawTile(cube,PK1,pos,TILE,0.11f,TILE,90, isNight); break;
                    case PPARKING1h:DrawTile(cube,PK1h,pos,TILE,0.11f,TILE,90, isNight); break;
                    case PPARKING2:DrawTile(cube,PK2,pos,TILE,0.11f,TILE,90, isNight); break;
                    case PPARKING2h:DrawTile(cube,PK2h,pos,TILE,0.11f,TILE,90, isNight); break;
                    case PLAYGROUND3:DrawTile(cube,PY3,pos,TILE,0.11f,TILE,180, isNight); break;
                    default: break;
                }
            }
        }
  
        // BUILDING DRAWING
        for (const Building &b : buildings)
{
    switch (b.type){
        case RESTAURANT: DrawTallBuilding( cube, b, 2, 3.0f, FREST, RBREST,B3F, FREST,B3F, B3F,B3F,B3F,B3R, isNight );
        break;   

        case BCOFFEE: DrawTallBuilding( cube, b, 3, 3.0f,RBBCOF,FBCOF, FBCOF,B4F, B4F, B4F,B4F,B4F,B4R,isNight);
        break;

        case HOSPITAL: DrawTallBuilding( cube, b, 2, 3.0f, HT, HTS2, HTS1 ,HTS1, HTS2, HTS2, HTS2 , HTS2, PHMR, isNight );
        break;

        case HOSPITAL1: DrawTallBuilding( cube, b, 2, 3.0f, FHT , HTS2, HTS1, HTS1,HTS2 , HTS2, HTS2,HTS2, PHMR, isNight );
        break;

        case HOSPITAL2: DrawTallBuilding( cube , b, 2, 3.0f, HT1, HTS1, FHT, FHT, HTS2, HTS2, HTS2, HTS2, PHMR, isNight );
        break;
        
        case HOSPITAL3: DrawTallBuilding( cube, b, 2, 3.0f, HTS2, PHMR, PHMR, FHT, HTS2, HTS2, HTS2,HTS2, PHMR, isNight );
        break;
        
        case PHARMACY: DrawTallBuilding( cube, b, 2, 3.0f, HTS2, PHM,  HT1 , PHM, HTS2, HTS2,  HTS2, HTS2, PHMR, isNight );
        break; 
        
        case BUILDING_b1: DrawTallBuilding( cube, b, 3, 3.0f, BF1,BS2, BS2, BS4, BF2, BS1,  BS1, BS3, BR, isNight );
        break;

        case FIRES : DrawBoxBuilding(cube, b, FS1, FRS, FRS, BF, FRS, isNight);
        break ;
        
        case FIRES1: DrawTallBuilding( cube, b, 2, 3.0f, FS3, FS, FS, FS1, FS, FS, FS, FS, FRS, isNight );
        break;    

        case FIRES2: DrawTallBuilding(  cube, b ,  2, 3.0f, BF, FS, FS, FS2, FS, FS, FS,FS,FRS, isNight);
        break; 

        case COFFE_S: DrawTallBuilding( cube, b, 2,  3.0f,  FCS, CSS, CSS,FCS, F1CS,  CSS, CSS, CS1, RCS, isNight  );
        break;

        case BAKERY: DrawTallBuilding( cube, b, 3, 3.0f,BK, RBY, BK,B2F , B2F, B2F, B2F2,B2F2,B2R, isNight  );
        break;

        case OFFICE_C: DrawTallBuilding( cube,  b,  3,  3.0f, B5F, B5F1, B5F1,B5F2, B5S1,B5F2, B5S1,B5S1, B5R, isNight );
        break;  
        
        case MINIMARKET: DrawTallBuilding( cube, b, 3, 3.0f, B6F1,MMS, MMF,MMF,B6F3,MMS,MMS,MMS,B6R,isNight );
        break;  


        case POLICES : DrawBoxBuilding(cube, b, FP, FP, SP, SP, RP, isNight);
        break ;

        case POLICES1: DrawTallBuilding( cube,b, 2, 3.0f,SP,S1P,S1P,SP,S1P,S1P,S1P,S1P,RP, isNight );
        break;    

        case POLICES2: DrawTallBuilding( cube, b, 2, 3.0f,SP,S1P, S1P,SP,S1P,S1P,S1P,S1P,RP,isNight );
        break; 

        case SCHOOL: DrawTallBuilding( cube,b,2,3.0f,FSLS,FSLS,FSLS, FSLS,FSLS,FSLS,FSLS,FSLS,FSLR, isNight );
        break; 

        case SCHOOL1: DrawTallBuilding( cube, b,2,3.0f, FSL,FSLS,FSLS,FSLS,F1SL,FSLS,FSLS,FSLS,FSLR,isNight );
        break;    

        case SCHOOL2: DrawTallBuilding( cube,b,2,3.0f,FSLS,FSLS,FSLS,FSLS,FSLS,FSLS,FSLS,FSLS,FSLR ,isNight );
        break;    
            
        case GSC: DrawBoxBuilding(cube, b, SGSC, FGSC, SGSC, FGSC, RGSC, isNight);
        break ;

        case BOOKSTORE:DrawBoxBuilding(cube, b, RBKS, SBKS, RBKS, FBKS, RBKS, isNight);
        break ;

        case BUILDING_b71: DrawTallBuilding( cube, b,2,3.0f,B7S,B7F, B7F2,B7S, B7S1,B7S,B7S1,B7S,B7R,isNight );
        break;

        case BUILDING_b72: DrawTallBuilding( cube, b,2,  3.0f, B7S, B7F2, B7F1,B7S, B7S1,B7S, B7S1, B7S,B7R, isNight);
        break;

        case BUILDING_b8: DrawTallBuilding( cube,  b,  2,  3.0f, B8F2, B8F2, B8F2,B8F,B8S2,B8S2, B8S, B8S,B8R, isNight );
        break;

        case BUILDING_b9: DrawTallBuilding( cube, b, 2,  3.0f, B9F1, B9S1, B9F,B9F,B9S,B9S, B9S,B9S, B9R, isNight );
        break;

        case BOUTIQUE:DrawBoxBuilding(cube, b, FBTQ, SBTQ, SBTQ, FBTQ, RBTQ, isNight);
        break ;

        default:
        break;
    }
}



   

        EndMode3D();
        DrawText("Emergency & Facility Demo READY", 20, 20, 20, DARKGREEN);
        EndDrawing();
    }


    // UNLOADING

    //RoadsElements
    UnloadTexture(g); UnloadTexture(RH); UnloadTexture(RV); UnloadTexture(PR); UnloadTexture(PR1);
    UnloadTexture(INT) ; UnloadTexture(RB); UnloadTexture(CVT); UnloadTexture(CVT1); UnloadTexture(CVB);
    UnloadTexture(FCVB);UnloadTexture(CVB1); UnloadTexture(TRD); UnloadTexture(TRD1); UnloadTexture(ROT); 
    UnloadTexture(PK);UnloadTexture(PK1); UnloadTexture(PK1h); UnloadTexture(PK2); UnloadTexture(PK2h); 
  
    //REstaurant
    UnloadTexture(FREST);UnloadTexture(RBREST); 
    
    //Black_CoffeShop
    UnloadTexture(FBCOF); UnloadTexture(RBBCOF); 
    
    //HospitalFacilities
    UnloadTexture(HT);UnloadTexture(HT1);UnloadTexture(PHM); UnloadTexture(PHMR);  UnloadTexture(FHT);
    UnloadTexture(HTS1); UnloadTexture(HTS2);

    //Buildings
    UnloadTexture(BF1); UnloadTexture(BF2); UnloadTexture(BS1); UnloadTexture(BS2); UnloadTexture(BS3);
    UnloadTexture(BS4);  UnloadTexture(BR);
        //Bakery
    UnloadTexture(B2F); UnloadTexture(B2F2); UnloadTexture(B2R); UnloadTexture(BK);UnloadTexture(RBY); 
        //Restaurant
    UnloadTexture(B3F);UnloadTexture(B3R);
        //Blackcoffe_shop
    UnloadTexture(B4F); UnloadTexture(B4R);
        //OfficeCenter
    UnloadTexture(B5S); UnloadTexture(B5S1); UnloadTexture(B5F);UnloadTexture(B5F1); UnloadTexture(B5F2); UnloadTexture(B5R);
        //Minimarket
    UnloadTexture(B6F1); UnloadTexture(B6F3);UnloadTexture(B6R); UnloadTexture(MMF); UnloadTexture(MMS);
        //CenterShop_Coffeshop
    UnloadTexture(B7F) ; UnloadTexture(B7F2) ; UnloadTexture(B7F1) ; UnloadTexture(B7R) ; UnloadTexture(B7S);
    UnloadTexture(B7S1);
        //Supermarket 
    UnloadTexture(B8F); UnloadTexture(B8F2);  UnloadTexture(B8S);  UnloadTexture(B8S2);UnloadTexture(B8R);  
        //Bookstore
    UnloadTexture(B9F);  UnloadTexture(B9F1);  UnloadTexture(B9S);  UnloadTexture(B9S1);UnloadTexture(B9R);

    //FireStation
    UnloadTexture(FS1); UnloadTexture(FS2); UnloadTexture(FRS); UnloadTexture(BF); UnloadTexture(FS);UnloadTexture(FCS); 

    //CoffeHouse
    UnloadTexture(F1CS); UnloadTexture(CSS); UnloadTexture(CS1); UnloadTexture(RCS);

    //PoliceStation
    UnloadTexture(RP); UnloadTexture(FP); UnloadTexture(SP); UnloadTexture(S1P); 
    
    //SchoolFacilities
    UnloadTexture(FSL);  UnloadTexture(F1SL); UnloadTexture(FSLS); UnloadTexture(FSLR);UnloadTexture(FGSC); 
    UnloadTexture(SGSC); UnloadTexture(RGSC); UnloadTexture(FBKS); UnloadTexture(SBKS); UnloadTexture(RBKS); 

    //Boutique
    UnloadTexture(FBTQ); UnloadTexture(SBTQ); UnloadTexture(RBTQ);

    UnloadModel(cube);
     
     
    CloseWindow();
    return 0;
}
