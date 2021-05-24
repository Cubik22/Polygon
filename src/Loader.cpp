#include "Loader.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

int Loader::SearchInFile(const std::string& fileName, const std::string& search, bool drawError){
    std::ifstream file;
    int found = OpenFileAndSearch(file, fileName, search, drawError);
    file.close();
    return found;
}

int Loader::GetNumberVerticesFromFile(const std::string& fileName){
    std::ifstream file;
    int found = OpenFileAndSearch(file, fileName, "number of vertices");
    if (found != 1){
        return found;
    }
    std::string line;
    std::stringstream convert;
    int numberVertices = 0;
    getline(file, line);
    convert.str(line);
    convert >> numberVertices;
    file.close();
    if (numberVertices <= 0 || convert.fail()){
        LOG(LogLevel::ERROR) << "number of vertices should be a number greather than 0";
        return -2;
    }
    return numberVertices;
}

int Loader::LoadSegmentFromFile(std::vector<Vector2f>& segmentPoints, const std::string& fileName){
    std::ifstream file;
    int found = OpenFileAndSearch(file, fileName, "segment");
    if (found != 1){
        return found;
    }
    std::string line;
    std::stringstream convert;
    for (unsigned int i = 0; i < 2; i++){
        getline(file, line);
        float x, y;
        convert.str(line);
        convert >> x >> y;
        if (convert.fail()){
            LOG(LogLevel::ERROR) << "problems when reading segment";
            LOG(LogLevel::ERROR) << "Line: " << line;
            segmentPoints.clear();
            return -3;
        }
        convert.clear();
        segmentPoints.emplace_back(x, y);
    }
    file.close();
    LOG(LogLevel::INFO) << "Correctly loaded segment from " << fileName;
    return 1;
}

int Loader::LoadVerticesFromFile(std::vector<Vector2f>& vertices, std::vector<unsigned int>& indices,
                                 const std::string& fileName, unsigned int numberVertices, bool loadIndices){
    if (numberVertices == 0){
        LOG(LogLevel::ERROR) << "number of vertices should be a number greather than 0";
        return -2;
    }
    std::ifstream file;
    OpenFileAndSearch(file, fileName, "vertices");
    std::string line;
    std::stringstream convert;
    vertices.reserve(numberVertices);
    indices.reserve(numberVertices);
    for (unsigned int i = 0; i < numberVertices; i++){
        getline(file, line);
        float x, y;
        convert.str(line);
        convert >> x >> y;
        if (convert.fail()){
            LOG(LogLevel::ERROR) << "problems when reading vertices";
            LOG(LogLevel::ERROR) << "Line: " << line;
            vertices.clear();
            indices.clear();
            return -3;
        }
        vertices.emplace_back(x, y);
        convert.clear();
        if (!loadIndices){
            indices.push_back(i);
        }
    }
    file.close();
    LOG(LogLevel::INFO) << "Correctly loaded vertices from " << fileName;

    if (!loadIndices){
        LOG(LogLevel::INFO) << "Correctly loaded default indices";
        return 1;
    }
    OpenFileAndSearch(file, fileName, "indices");
    getline(file, line);
    unsigned int index;
    convert.str(line);

    for (unsigned int i = 0; i < numberVertices; i++){
        convert >> index;
        if (convert.fail()){
            LOG(LogLevel::ERROR) << "problems when reading indices";
            LOG(LogLevel::ERROR) << "Line: " << line;
            vertices.clear();
            indices.clear();
            return -3;
        }
        indices.push_back(index);
    }

    file.close();
    LOG(LogLevel::INFO) << "Correctly loaded indices from " << fileName;
    return 1;
}

void Loader::SavePolygonToFile(const std::vector<Vector2f>& vertices, const std::vector<unsigned int>& indices,
                               const std::vector<Vector2f>& segmentPoints,
                               const std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices,
                               const std::string& fileName){
    LOG(LogLevel::INFO) << "Saving polygon to " << fileName;
    unsigned int numberVertices = vertices.size();
    std::ofstream file;
    file.open(fileName);
    if (file.fail()){
        LOG(LogLevel::ERROR) << "failed to open file " << fileName;
        return;
    }
    file << "# segment\n";
    file << segmentPoints[0].x << " " << segmentPoints[0].y << "\n";
    file << segmentPoints[1].x << " " << segmentPoints[1].y << "\n";
    file << "# number of vertices\n";
    file << numberVertices << "\n";
    file << "# indices\n";
    for (unsigned int i = 0; i < numberVertices; i++){
        file << indices[i] << " ";
    }
    file << indices[2 * (numberVertices - 1)] << "\n";
    file << "# vertices\n";
    for (unsigned int i = 0; i < numberVertices; i++){
        file << vertices[i].x << " " << vertices[i].y << "\n";
    }
    if (polygonsIndices.size() > 0){
        file << "# number of small polygons\n";
        unsigned int numberSmallPolygons = polygonsIndices.size();
        file << numberSmallPolygons << "\n";
        file << "# small polygons indices\n";
        for (unsigned int i = 0; i < numberSmallPolygons; i++){
            const std::vector<unsigned int>& smallIndices = *(polygonsIndices[i]);
            for (unsigned int n = 0; n < smallIndices.size(); n++){
                file << smallIndices[n] << " ";
            }
            file << "\n";
        }
    }
    file.close();
    LOG(LogLevel::INFO) << "Polygon correctly saved";
}

int Loader::GetNumberSmallPolygonsFromFile(const std::string& fileName){
    std::ifstream file;
    int found = OpenFileAndSearch(file, fileName, "number of small polygons");
    if (found != 1){
        return found;
    }
    std::string line;
    std::stringstream convert;
    int numberSmallPolygons = 0;
    getline(file, line);
    convert.str(line);
    convert >> numberSmallPolygons;
    file.close();
    if (numberSmallPolygons <= 0 || convert.fail()){
        LOG(LogLevel::ERROR) << "number of small polygons should be a number greather than 0";
        return -2;
    }
    return numberSmallPolygons;
}

int Loader::LoadSmallPolygonsIndicesFromFile(std::vector<std::shared_ptr<std::vector<unsigned int>>>& polygonsIndices,
                                             const std::string& fileName, unsigned int numberSmallPolygons){
    if (numberSmallPolygons == 0){
        LOG(LogLevel::ERROR) << "number of small polygons should be a number greather than 0";
        return -2;
    }
    std::ifstream file;
    OpenFileAndSearch(file, fileName, "small polygons indices");
    std::string line;
    std::stringstream convert;
    polygonsIndices.clear();
    unsigned int index;
    for (unsigned int i = 0; i < numberSmallPolygons; i++){
        getline(file, line);
        convert.str(line);
        polygonsIndices.push_back(std::make_shared<std::vector<unsigned int>>());
        while ((convert >> index) && !convert.eof()){
            if (convert.fail()){
                LOG(LogLevel::ERROR) << "problems when reading small polygons indices";
                LOG(LogLevel::ERROR) << "Line: " << line;
                polygonsIndices.clear();
                return -3;
            }
            polygonsIndices[i]->push_back(index);
        }
        convert.clear();
    }
    file.close();
    LOG(LogLevel::INFO) << "Correctly loaded small polygons indices from " << fileName;
    return 1;
}

int Loader::OpenFileAndSearch(std::ifstream& file, const std::string fileName, const std::string& search, bool drawError){
    file.open(fileName);
    if (file.fail()){
        if (drawError){
            LOG(LogLevel::ERROR) << "failed to open file " << fileName;
        }
        return 0;
    }
    std::string searchLine = "# " + search;
    std::string line;
    getline(file, line);
    while (line != searchLine){
        getline(file, line);
        if (file.eof()){
            if (drawError){
                LOG(LogLevel::ERROR) << "failed to load " << search;
            }
            return -1;
        }
    }
    return 1;
}
