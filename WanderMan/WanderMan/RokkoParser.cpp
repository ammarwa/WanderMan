#include "RokkoParser.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include<iostream>
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <string>

vector<std::string> RokkoParser::explode(string aStr, char aDelim)
{
    vector<std::string> res;
    string str = aStr.substr(0, aStr.find(aDelim));
    
    while(str.size() < aStr.size())
    {
        res.push_back(str);
        aStr = aStr.substr(aStr.find(aDelim) + 1);
        str = aStr.substr(0, aStr.find(aDelim));
    }
    
    res.push_back(str);
    
    return res;
}

void RokkoParser::objToTxt(const string aInFilename,
                           const string aOutFilename,
                           bool aVerbose)
{
    if(aVerbose) cout << "Loading OBJ file <"
        << aInFilename << ">" << endl;
    
    // Open file
    ifstream objFile(aInFilename.c_str());
    
    if(objFile.fail())
    {
        cout << "Error: could not open file <"
        << aInFilename << ">" << endl;
        exit(1);
    }
    
    
    // Extract verts, normals, textures, and faces
    vector<std::string> verts, norms, textures;
    vector<std::string> faces;
    map faceHash;
    
    vector<std::string> finalVerts, finalNorms, finalTextures;
    vector<std::string> finalFaces;
    
    string line;
    int hashIndex = 0;
    
    if(aVerbose) cout << "Extracting values from file" << endl;
    
    // Visit each line of the obj file
    while(getline(objFile, line))
    {
        // Extract vertex
        // Line starts with v[space]...
        if(line[0] == 'v' && line[1] == ' ')
        {
            string lineVals = line.substr(2);
            float val;
            
            string val0 = lineVals.substr(0, lineVals.find(' '));
            val = (float)atof(val0.c_str());
            verts.push_back(val);
            
            string val1 = lineVals.substr(val0.length() + 1,
                                          lineVals.find(' '));
            val = (float)atof(val1.c_str());
            verts.push_back(val);
            
            string val2 = lineVals.substr(lineVals.find_last_of(' ') + 1);
            val = (float)atof(val2.c_str());
            verts.push_back(val);
        }
        
        
        // Extract textures
        // Line starts with vt[space]...
        else if(line[0] == 'v' && line[1] == 't' && line[2] == ' ')
        {
            string lineVals = line.substr(3);
            float val;
            
            string val0 = lineVals.substr(0, lineVals.find(' '));
            val = (float)atof(val0.c_str());
            textures.push_back(val);
            
            string val1 = lineVals.substr(val0.length() + 1,
                                          lineVals.find(' '));
            val = (float)atof(val1.c_str());
            textures.push_back(val);
        }
        
        
        // Extract normals
        // Line starts with vn[space]...
        else if(line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
        {
            string lineVals = line.substr(3);
            float val;
            
            string val0 = lineVals.substr(0, lineVals.find(' '));
            val = (float)atof(val0.c_str());
            norms.push_back(val);
            
            string val1 = lineVals.substr(val0.length() + 1,
                                          lineVals.find(' '));
            val = (float)atof(val1.c_str());
            norms.push_back(val);
            
            string val2 = lineVals.substr(lineVals.find_last_of(' ') + 1);
            val = (float)atof(val2.c_str());
            norms.push_back(val);
        }
        
        
        //
        // 2. Hash faces
        //
        // Extract faces
        // Line starts with f[space]...
        else if(line[0] == 'f' && line[1] == ' ')
        {
            string lineVals = line.substr(2);
            
            string val0 = lineVals.substr(0, lineVals.find_first_of(' '));
            
            // If the value for this face includes texture and/or
            // normal, parse them out
            if(val0.find('/') >= 0)
            {
                // Get first group of values
                string g1 = val0.substr(0, val0.find(' '));
                
                // Get second group of values
                string g2 = line.substr(line.find(' ') + 2);
                g2 = g2.substr(g2.find(' ') + 1);
                g2 = g2.substr(0, g2.find(' '));
                
                string g3 = line.substr(line.find_last_of(' ') + 1);
                
                if(aVerbose)
                    cout << "Face: (" << g1 << ") (" << g2 << ") (" << g3 << ")" << endl;
                
                // Just stick all the unique values in this hash and give each key a
                // unique, increasing value
                map::iterator itr;
                
                //
                // Add key's position to the faces list
                //
                
                itr = faceHash.find(g1);
                // If key not in map
                if(itr == faceHash.end())
                {
                    faceHash[g1] = hashIndex++;
                }
                itr = faceHash.find(g1);
                faces.push_back(itr->second);
                
                itr = faceHash.find(g2);
                // If key not in map
                if(itr == faceHash.end())
                {
                    faceHash[g2] = hashIndex++;
                }
                itr = faceHash.find(g2);
                faces.push_back(itr->second);
                
                itr = faceHash.find(g3);
                // If key not in map
                if(itr == faceHash.end())
                {
                    faceHash[g3] = hashIndex++;
                }
                itr = faceHash.find(g3);
                faces.push_back(itr->second);
            }
            
            // Only verts in file
            else
            {
                // Push faces straight up -- converting to
                // base zero in the process
            }
        }
    } /* end getline(file, line) */
    
    if(aVerbose) cout  << "Finished extracting values from file" << endl
        << "Quick count check:" << endl
        << "\tVerts = " << verts.size() << endl
        << "\tNorms = " << norms.size() << endl
        << "\tTexts = " << textures.size() << endl
        << "\tFaces = " << faces.size() << endl;
    
    objFile.close();
    
    if(aVerbose) cout << "Preparing to build faces" << endl;
    
    //
    // 3. Fill verts, texts, and norms lists so it can be indexed directly.
    //   Length = hash.size
    //
    for(int i = 0; i < faceHash.size() * 2; i++)
        finalTextures.push_back(0.0f);
    
    for(int i = 0; i < faceHash.size() * 3; i++)
    {
        finalVerts.push_back(0.0f);
        finalNorms.push_back(0.0f);
    }
    
    
    // 5. Walk through hash, extract each value in current key
    // * Remember to make faces array zero based
    //
    // Cases for keys:
    //    1. ## ## ## (verts only)
    //    2. ##/## ##/## ##/## (verts and textures)
    //    3. ##//## ##//## ##//## (verts and normals)
    //    4. ##// ##// ##// (verts only)
    //
    
    if(aVerbose) cout << "Hashing list of unique vertices" << endl;
    
    if(aVerbose) cout << faceHash.size() << " unique vertices found" << endl;
    
    map::iterator hashItr = faceHash.begin();
    int faceCounter = 0;
    while(hashItr != faceHash.end())
    {
        string faceHashKey = hashItr->first;
        int faceHashVal = hashItr->second;
        
        if(aVerbose) cout << "Unique face #" << faceHashVal
            << " = " << faceHashKey << endl;
        
        // Default values
        float v0 = (float)0.0f;
        float v1 = (float)0.0f;
        float v2 = (float)0.0f;
        
        float t0 = (float)0.0f;
        float t1 = (float)0.0f;
        
        float n0 = (float)0.0f;
        float n1 = (float)0.0f;
        float n2 = (float)0.0f;
        
        vector vals = RokkoParser::explode(faceHashKey, '/');
        
        v0 = (float)verts[(atoi(vals[0].c_str()) - 1) * 3];
        v1 = (float)verts[(atoi(vals[0].c_str()) - 1) * 3 + 1];
        v2 = (float)verts[(atoi(vals[0].c_str()) - 1) * 3 + 2];
        
        if(vals.size() > 1 && vals[1].size() > 0)
        {
            t0 = (float)textures[(atoi(vals[1].c_str()) - 1) * 2];
            t1 = (float)textures[(atoi(vals[1].c_str()) - 1) * 2 + 1];
        }
        
        if(vals.size() > 2 && vals[2].size() > 0)
        {
            n0 = (float)norms[(atoi(vals[2].c_str()) - 1) * 3];
            n1 = (float)norms[(atoi(vals[2].c_str()) - 1) * 3 + 1];
            n2 = (float)norms[(atoi(vals[2].c_str()) - 1) * 3 + 2];
        }
        
        
        finalVerts.at(faceHashVal * 3) = v0;
        finalVerts.at(faceHashVal * 3 + 1) = v1;
        finalVerts.at(faceHashVal * 3 + 2) = v2;
        
        finalTextures.at(faceHashVal * 2) = t0;
        finalTextures.at(faceHashVal * 2 + 1) = t1;
        
        finalNorms.at(faceHashVal * 3) = n0;
        finalNorms.at(faceHashVal * 3 + 1) = n1;
        finalNorms.at(faceHashVal * 3 + 2) = n2;
        
        
        if(aVerbose) cout  << "  Vert: " << finalVerts.at(faceHashVal * 3)
            << " " << finalVerts.at(faceHashVal * 3 + 1)
            << " "
            << finalVerts.at(faceHashVal * 3 + 2)
            << "  Text: " << finalTextures.at(faceHashVal * 2)
            << " "
            << finalTextures.at(faceHashVal * 2 + 1)
            << "  Norm: " << finalNorms.at(faceHashVal * 3)
            << " "
            << finalNorms.at(faceHashVal * 3 + 1)
            << " " 
            << finalNorms.at(faceHashVal * 3 + 2)
            << endl;
        
        hashItr++;
    }
    
    ofstream out(aOutFilename.c_str());
    
    if(out.fail())
    {
        cout << "Error: could not create output file " << aOutFilename << endl;
        exit(1);
    }
    
    if(aVerbose) cout << "Saving data to " << aOutFilename << endl;
    
    int len;
    
    // Verts
    out << "{"
    << "\"Verts\": [";
    
    len = finalVerts.size();
    for(int i = 0; i < len; i++)
    {
        out << finalVerts[i];
        
        if(i < len - 1)
            out << ",";
    }
    
    out << "]";
    
    // Normals
    out << ",\"Normals\": [";
    
    len = finalNorms.size();
    for(int i = 0; i < len; i++)
    {
        out << finalNorms[i];
        
        if(i < len - 1)
            out << ",";
    }
    
    out << "]";
    
    
    // Textures
    out << ",\"Textures\": [";
    
    len = finalTextures.size();
    for(int i = 0; i < len; i++)
    {
        out << finalTextures[i];
        
        if(i < len - 1)
            out << ",";
    }
    
    out << "]";
    
    
    // Textures
    out << ",\"Textures\": [";
    
    len = finalTextures.size();
    for(int i = 0; i < len; i++)
    {
        out << finalTextures[i];
        
        if(i < len - 1)
            out << ",";
    }
    
    out << "]";
    
    
    // Faces
    out << ",\"Faces\": [";
    
    len = faces.size();
    for(int i = 0; i < len; i++)
    {
        out << faces[i];
        
        if(i < len - 1)
            out << ",";
    }
    
    out << "]";
    
    
    out << "}";
    
    if(aVerbose) cout << "All done!" << endl;
    out.close();
}