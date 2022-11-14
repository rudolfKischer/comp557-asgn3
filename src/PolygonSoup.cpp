#include "PolygonSoup.h"

PolygonSoup::PolygonSoup(std::string filename)
{
    try
    {
        std::ifstream ifs(filename);
        if (!ifs) {
            cout << "Cannot open file [" << filename << "]" << std::endl;
        }

        std::string linebuf;
        while (ifs.peek() != -1) {
            safeGetline(ifs, linebuf);
            if (linebuf[0] == 'v' && linebuf[1] == ' ') {
                parseVertex(linebuf);
            }
            else if (linebuf[0] == 'f' && linebuf[1] == ' ') {
                parseFace(linebuf);
            }
        }

        soupStatistics = filename + "\n" + "faces = " + std::to_string(faceList.size()) + "\nverts = " + std::to_string(vertexList->size()) + "\n";
        for (auto const &e : faceSidesHistogram) {
            soupStatistics += to_string(e.second) + " ";
            if (e.first == 3) {
                soupStatistics += "triangles\n";
            }
            else if (e.first == 4) {
                soupStatistics += "quadrilaterals\n";
            }
            else {
                soupStatistics += e.first + "-gons\n";
            }
        }

        std::cout << soupStatistics;

        /*
         * TODO: 1 compute a bounding box and scale and center the geometry here
         */
        
        //compute bounding box
        glm::vec3 lbn = vertexList->at(0)->p;// bottom left near corner
        glm::vec3 rtf = vertexList->at(0)->p;// right top far corner
        for (int i=0; i<vertexList->size(); i++) {
            for (int j=0; j<3; j++ ){
                glm::vec3 v = vertexList->at(i)->p;
                if( v[j] < lbn[j]){
                    lbn[j] = v[j];
                }
                
                if( v[j] > rtf[j]){
                    rtf[j] = v[j];
                }
            }
//            vertexList->at(i)->p = 5.0f * vertexList->at(i)->p ;
        }
        
        //calculate largest dimension
        glm::vec3 center = lbn + (0.5f* (rtf - lbn));
        glm::vec3 lwh = (rtf - lbn);//length ,width ,height
        int maxDim=0;//axis of largest length of the bounding box
        for (int i=0; i<3;i++){
            if(lwh[i] > lwh[maxDim]){
                maxDim = i;
            }
        }
        
        //calculate scaling factor
        float maxLength = 10.0f;
        float scalingFactor = maxLength / lwh[maxDim];
        
        
        //scale and translate
        for (int i=0; i<vertexList->size(); i++) {
            vertexList->at(i)->p = vertexList->at(i)->p - center;
            vertexList->at(i)->p = scalingFactor * vertexList->at(i)->p ;
        }

        // reserve buffer data
        this->norBuf.reserve(faceList.size() * 3);
        this->posBuf.reserve(faceList.size() * 3 * 3);

        // assume triangular faces when drawing
        glm::vec3 v1;
        glm::vec3 v2;
        glm::vec3 n;

        int indexCounter = 0;
        for (auto &faceVertex : faceList) {
            indexCounter++;
            glm::vec3 p0 = vertexList->at(faceVertex[0])->p;
            glm::vec3 p1 = vertexList->at(faceVertex[1])->p;
            glm::vec3 p2 = vertexList->at(faceVertex[2])->p;
            v1 = p1 - p0;
            v2 = p2 - p1;
            n = glm::cross(v1, v2);
            n = glm::normalize(n);
            if (isnan(n.x) || isnan(n.y) || isnan(n.z)) {
                n.x = 0;
                n.y = 0;
                n.z = 0;
            }
            this->norBuf.push_back(n.x);
            this->norBuf.push_back(n.y);
            this->norBuf.push_back(n.z);
            for (int i = 0; i < faceVertex.size(); i++) {
                glm::vec3 p = vertexList->at(faceVertex[i])->p;
                this->posBuf.push_back(p.x);
                this->posBuf.push_back(p.y);
                this->posBuf.push_back(p.z);
            }
        }

        // bind buffers

        glGenVertexArrays(1, &vao);

        // Send the position array to the GPU
        glGenBuffers(1, &posBufID);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID);
        glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);

        // Send the normal array to the GPU
        if (!norBuf.empty()) {
            glGenBuffers(1, &norBufID);
            glBindBuffer(GL_ARRAY_BUFFER, norBufID);
            glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
        }

        // Unbind the arrays
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    catch (std::string e)
    {
        std::cout << e << " failed to read.\n Please check the path." << std::endl;
        exit(1);
    }
}

Vertex *PolygonSoup::parseVertex(std::string newline)
{
    vector<string> tokens;
    stringstream check1(newline);
    string intermediate;

    // Tokenizing w.r.t. space ' '
    while (getline(check1, intermediate, ' ')) {
        if (intermediate[0] != '\0') {
            tokens.push_back(intermediate);
        }
    }

    Vertex *v = new Vertex();
    // ignore the tag "v " at index 0
    v->p.x = stof(tokens[1]);
    v->p.y = stof(tokens[2]);
    v->p.z = stof(tokens[3]);
    v->index = (int)vertexList->size();
    vertexList->emplace_back(v);
    return v;
}

std::vector<int> PolygonSoup::parseFace(std::string newline)
{
    // vertex/texture/normal tuples are separated by a spaces.

    vector<string> tokens;
    stringstream check1(newline);
    string intermediate;

    // Tokenizing w.r.t. space ' '
    while (getline(check1, intermediate, ' ')) {
        if (intermediate[0] != '\0') {
            tokens.push_back(intermediate);
        }
    }

    int size = tokens.size();
    vector<int> v;
    for (int i = 1; i < size; i++) // ignore the tag "f " at i=0
    {
        // first token is vertex index... we'll ignore the rest
        v.push_back(stoi(tokens[i]) - 1); // want zero indexed vertices!
    }

    int count = size - 1;

    int finding = (int)faceSidesHistogram.count(count);
    if (finding == 0)
    {
        faceSidesHistogram.emplace(count, 1);
    }
    else
    {
        auto existing = faceSidesHistogram.find(count);
        existing->second++;
    }
    faceList.push_back(v);
    return v;
}

void PolygonSoup::display(shared_ptr<Program> prog, shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV)
{
    prog->bind();

    glBindVertexArray(vao);

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
    glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);

    // Bind position buffer
    int h_pos = prog->getAttribute("aPos");
    glEnableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

    glUniform3f(prog->getUniform("col"), 0.1f, 0.1f, 0.1f);

    // Draw
    int count = posBuf.size() / 3; // number of indices to be rendered
    glDrawArrays(GL_TRIANGLES, 0, count);

    // Disable and unbind
    glDisableVertexAttribArray(h_pos);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    prog->unbind();
}
