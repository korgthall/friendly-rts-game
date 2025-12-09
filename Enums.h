#ifndef THE_GLORBZ_ENUMS_H
#define THE_GLORBZ_ENUMS_H
#define SIZE 20

enum Tasks {
    GATHERGOLD,
    GATHERGLORBIUM,
    BUILD,
    ATTACK,
    IDLE,
    REPAIR,
    MOVING
};

enum ResourceType {
    GOLD,
    GLORBIUM
};

enum BuildingType {
    HUB,
    BARRACKS,
    GLORBSTRACTOR
};
#endif //THE_GLORBZ_ENUMS_H