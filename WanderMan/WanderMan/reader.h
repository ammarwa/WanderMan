//
//  reader.h
//  WanderMan
//
//  Created by Ammar Mohamed ELWazir on 12/6/14.
//  Copyright (c) 2014 Ammar Mohamed ELWazir. All rights reserved.
//

#ifndef READER_H
#define READER_H

#define MAX_VERT 100000

struct Vertex {
    
    float x;
    float y;
    float z;
    
};


class Reader {
private:
    Vertex vertex[256];
    
public:
    Reader();
    ~Reader();
    
    void load(char* filename);
    void draw(char* filename);
    
    
};

#endif
