#pragma once
#ifndef RELATIONSHIPTYPE_H
#define RELATIONSHIPTYPE_H

enum class RelationshipType {
    Follows,
    FollowsT,
    Parent,
    ParentT,
    Uses,
    Modifies,
    Null
};

#endif