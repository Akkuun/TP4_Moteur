#include <vector>
#include "Node_3D.hpp"

class SceneManager {
private:
    std::vector<Node_3D*> gameObject;

public:
    void add(Node_3D* node){
        node->setIdObject(gameObject.size());
        gameObject.push_back(node);  // Ajouter l'objet pointé par node à gameObject
    }
    void initScene(GLuint programID){
        for(int i = 0; i < gameObject.size(); i++){
            gameObject[i]->initBufferObject(programID);
        }

    }
    void removeById(int id) {
        for (auto it = gameObject.begin(); it != gameObject.end(); ++it) {
            if ((*it)->getIdObject() == id) {
                gameObject.erase(it);
                updateIds();
                return;
            }
        }
    }
    void removeLast(){
        gameObject.pop_back();
    }
    void updateIds() {
        for (int i = 0; i < gameObject.size(); ++i) {
            gameObject[i]->setIdObject(i);
        }
    }
    void drawScene(GLuint programID){
        for(int i = 0; i < gameObject.size(); i++){
            gameObject[i]->draw();
        }
        
    }
};
