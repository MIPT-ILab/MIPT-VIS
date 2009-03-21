/**
 * File: node.cpp - Node class implementation, part of Graph library, 
 * an internal representation of graphs in MiptVis tool.
 * Copyright (C) 2009  Boris Shurygin
 */
#include "graph_impl.h"

/** 
 * Destructor. Corrects list of nodes in corresponding graph and deletes adjacent edges
 */
Node::~Node()
{
    Edge *edge;
    
    /** delete incidient edges */
    for ( edge = firstSucc(); !endOfSuccs();)
    {
        Edge* next = nextSucc();
        edge->detachFromNode( GRAPH_DIR_DOWN);// Edge is detached from succ node
        delete edge;
        edge = next;
    }
    for ( edge = firstPred(); !endOfPreds();)
    {
        Edge* next = nextPred();
        edge->detachFromNode( GRAPH_DIR_UP);// Edge is detached from pred node
        delete edge;
        edge = next;
    }
    /** delete myself from graph */
    graph->deleteNode( &my_it);
}

/**
 * Add an edge to this node in specified direction
 */
void
Node::addEdgeInDir( Edge *edge, GraphDir dir)
{
    graphassert( isNotNullP( edge));
    EdgeListItem *it = edge->getNodeIt( revDir( dir));
    it->Attach( edges[ dir]);
    edges[ dir] = it;
}

/**
 * delete edge pointed by iterator in specidied direction
 */
void
Node::deleteEdgeInDir( GraphDir dir, EdgeListItem* it)
{
    graphassert( isNotNullP( it));
    if( edges[ dir] == it)
    {
        edges[ dir] = it->next();
    }
    if( e_it[ dir] == it)
    {
        e_it[ dir] = it->next();
    }
    it->Detach();
}

/**
 * Print node in Dot format to stdout
 */
void
Node::debugPrint()
{
    out("%u;", id());
}

/**
 *  Write to xml
 */
void
Node::writeByXMLWriter( xmlTextWriterPtr writer)
{
	xmlTextWriterWriteElement( writer, BAD_CAST "label", BAD_CAST label());
}