set(cp_sources
    chipmunk.c
    cpArbiter.c
    cpArray.c
    cpBBTree.c
    cpBody.c
    cpCollision.c
    cpConstraint.c
    cpDampedRotarySpring.c
    cpDampedSpring.c
    cpGearJoint.c
    cpGrooveJoint.c
    cpHashSet.c
    cpHastySpace.c
    cpMarch.c
    cpPinJoint.c
    cpPivotJoint.c
    cpPolyShape.c
    cpPolyline.c
    cpRatchetJoint.c
    cpRobust.c
    cpRotaryLimitJoint.c
    cpShape.c
    cpSimpleMotor.c
    cpSlideJoint.c
    cpSpace.c
    cpSpaceComponent.c
    cpSpaceDebug.c
    cpSpaceHash.c
    cpSpaceQuery.c
    cpSpaceStep.c
    cpSpatialIndex.c
    cpSweep1D.c
)

add_library(chipmunk_static STATIC ${cp_sources})
set_target_properties(chipmunk_static PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "chipmunk"
)

if(NOT TARGET chipmunk)
    add_library(chipmunk ALIAS chipmunk_static)
endif() 