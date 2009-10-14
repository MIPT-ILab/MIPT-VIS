/** 
* File: graph_aux.cpp - Auxiliary graph class implementation, part of
* layout library, internal graph placement component in MiptVis tool.
* Copyright (C) 2009 MiptVis
*/

#include "layout_iface.h"


NodeAux* GraphAux::getFirstNode()
{
	return addAux (firstNode());
}

/**
* Make graph acyclic and set to all edges corresponding type
*/
NodeAux* GraphAux::makeAcyclic()
{
	NodeAux* first = getFirstNode();
	if (first == 0) return 0;
	
	Marker unidir = newMarker();
	Marker ret = newMarker();

	makeAcyclicImp (first, unidir, ret);

	freeMarker (unidir);
	freeMarker (ret);

	return first;
}
//-----------------------------------------------------------------------------
//Internal implementation, one mustn't call from outside
void GraphAux::makeAcyclicImp (NodeAux* from, Marker passed, Marker ret)
{
	from->mark (passed);

	for (EdgeAux* cur = from->firstSucc(); !from->endOfSuccs();
		          cur = from->nextSucc())
		if (cur->succ() == from) cur->type = EdgeAux::mesh; //mesh
		else
		{
			if (cur->succ()->isMarked (passed))
			{
				if (cur->succ()->isMarked (ret))
					 cur->type = EdgeAux::forward;			//forward
				else cur->type = EdgeAux::back;				//back
			}
			else
			{
				cur->type = EdgeAux::tree;					//tree
				makeAcyclicImp (addAux (cur->succ()), passed, ret);
			}
		}

	from->mark (ret);
}
//-----------------------------------------------------------------------------
/**
* Ranking algorithm. Divide nodes to layers
*/
bool GraphAux::rank()
{
	NodeAux* root = makeAcyclic();
	if (root == 0) return false;
	
	rankImp (root, 0);

	addVirtualChains();

	return true;
}
//-----------------------------------------------------------------------------
//Internal implementation, ranks nodes by the longest path for it
void GraphAux::rankImp (NodeAux* from, int cur_rank)
{
	from->rang_priv = cur_rank;
	for (EdgeAux* cur = from->firstSucc(); !from->endOfSuccs();//excess all tree edges
		          cur = from->nextSucc())
	{
		if (cur->type == EdgeAux::tree && cur->succ()->rang() <= cur_rank)
			rankImp (addAux (cur->succ()), cur_rank + 1);//climb to a max-lenght path
	}
	
	for (EdgeAux* cur = from->firstPred(); !from->endOfPreds();//excess all back edges
		          cur = from->nextPred())
	{
		if (cur->type == EdgeAux::back && cur->succ()->rang() <= cur_rank)
			rankImp (addAux (cur->pred()), cur_rank + 1);//climb to a max-lenght path
	}
}
//-----------------------------------------------------------------------------

void GraphAux::addVirtualChains()
{
	for(EdgeAux* iter = firstEdge(); iter != NULL; iter = iter->nextEdge())
	{
		int rang_pred = iter->pred()->rang_priv, rang_succ = iter->succ()->rang_priv;
		if( abs(rang_pred - rang_succ) > 1)
		{
			NodeAux* n = insertNodeOnEdge( iter);
			if( rang_pred > rang_succ)
				n->rang_priv = rang_pred - 1;
			else
				n->rang_priv = rang_pred + 1;
		}
	}
}