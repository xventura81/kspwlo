/*
Copyright (c) 2017 Theodoros Chondrogiannis
*/

#include "kspwlo.hpp"

Path next_spwlo_bounds(RoadNetwork *rN, NodeID source, NodeID target, double theta, unordered_map<Edge, vector<int>,boost::hash<Edge>> &resEdges, vector<Path> &resPaths, vector<int> &bounds);

/*
 *
 *	multipass(RoadNetwork*, NodeID, NodeID, int, double)
 *	-----
 *	Implementation of the MultiPass algorithm.
 * 	Calls function next_spwlo_bounds
 *
 */ 

vector<Path> multipass(RoadNetwork *rN, NodeID source, NodeID target, unsigned int k, double theta) {
   	int count = 0;
    Edge edge;
    unordered_map<Edge, vector<int>,boost::hash<Edge>> resEdges;
    unordered_map<Edge, vector<int>,boost::hash<Edge>>::iterator iterE;
	
	vector<Path> resPaths;
	pair<Path,vector<int>> resDijkstra= dijkstra_path_and_bounds(rN,source,target);
	
    Path resNext = resDijkstra.first;

    if(resNext.nodes.empty())
        return resPaths;
    
    resPaths.push_back(resNext);
    
    if(k==1)
		return resPaths;
    
    for(unsigned int i=1;i<k;i++) {
    		
		for(unsigned int j=0; j < resNext.nodes.size()-1; j++) {
        	edge = make_pair(resNext.nodes[j],resNext.nodes[j+1]);
        	if ((iterE = resEdges.find(edge)) == resEdges.end())
         	   resEdges.insert(make_pair(edge, vector<int>(1, count)));
       	 	else
        	    iterE->second.push_back(count);
    	}
    	count++;
    	
        resNext = next_spwlo_bounds(rN, source, target, theta, resEdges, resPaths, resDijkstra.second);
        
		if(resNext.length == -1)
			break;

		resPaths.push_back(resNext);
    }
    
    return resPaths;
}

/*
	next_spwlo_bounds(RoadNetwork, NodeID, NodeID, double, unordered_map<Edge, vector<int>,boost::hash<Edge>>, vector<Path>, vector<int>)
	-----
	This is the internal function called by MultiPass to produce the shortest
	alternative to the provided set of paths.
*/

Path next_spwlo_bounds(RoadNetwork *rN, NodeID source, NodeID target, double theta, unordered_map<Edge, vector<int>,boost::hash<Edge>> &resEdges, vector<Path> &resPaths, vector<int> &bounds) {
	Path resPath;
	resPath.length = -1;
	PriorityQueueAS2 Q;
    SkylineContainer skyline;
    int newLength = 0;
    vector<double> newOverlap;
    EdgeList::iterator iterAdj;
    unordered_map<Edge, vector<int>,boost::hash<Edge>>::iterator iterE;
    Edge edge;
    bool check = true;
    vector<OlLabel*> allCreatedLabels;
   	
    newOverlap.resize(resPaths.size(), 0);
    int newLowerBound = bounds[source];
    Q.push(new OlLabel(source, newLength, newLowerBound, newOverlap, -1));
    
    while (!Q.empty()) {
        OlLabel *curLabel = static_cast<OlLabel*> (Q.top());
        Q.pop();
                
        // Found target.
        if (curLabel->node_id == target) {
            OlLabel *tempLabel = curLabel;
        	
        	while(tempLabel != NULL) {
               	resPath.nodes.push_back(tempLabel->node_id);
    			tempLabel = static_cast<OlLabel*> (tempLabel->previous);
    		}
    		reverse(resPath.nodes.begin(),resPath.nodes.end());
            resPath.length = curLabel->length;
            break;
        }
        if(skyline.dominates(curLabel))
            continue;
		skyline.insert(curLabel);
		
        // Expand search. For each outgoing edge.
        for(iterAdj = rN->adjListOut[curLabel->node_id].begin(); iterAdj != rN->adjListOut[curLabel->node_id].end(); iterAdj++) {

            if(curLabel->previous !=NULL && curLabel->previous->node_id == iterAdj->first) 
            	continue;
            
            newLength = curLabel->length + iterAdj->second;
            newOverlap = curLabel->overlapList;
            newLowerBound = newLength + bounds[iterAdj->first];
            OlLabel* newPrevious = curLabel;
            edge = make_pair(curLabel->node_id,iterAdj->first);
            check = true;
        	   
            if ((iterE = resEdges.find(edge)) != resEdges.end()) {
            	for(unsigned int j = 0; j < iterE->second.size(); j++) {
                    newOverlap[iterE->second[j]] += iterAdj->second;
            		if (newOverlap[iterE->second[j]]/resPaths[iterE->second[j]].length > theta) {
                        check = false;
                        break;
                	}
                }
            }
        	   
            if (check) {
            	OlLabel *label = new OlLabel(iterAdj->first, newLength, newLowerBound, newOverlap, -1,newPrevious);
                Q.push(label);
                allCreatedLabels.push_back(label);     
            } 
        }
    }
    
    for(unsigned int i=0;i<allCreatedLabels.size();i++)
    	delete allCreatedLabels[i];
	
    return resPath;
}