/***************************************************************************
               starblockfactory.cpp  -  K Desktop Planetarium
                             -------------------
    begin                : 7 Jun 2008
    copyright            : (C) 2008 by Akarsh Simha
    email                : akarshsimha@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "starblockfactory.h"

#include "starblock.h"
#include "starobject.h"

#include <QDebug>

// TODO: Implement a better way of deciding this
#define DEFAULT_NCACHE 12

StarBlockFactory *StarBlockFactory::pInstance = 0;

StarBlockFactory *StarBlockFactory::Instance()
{
    if (!pInstance)
        pInstance = new StarBlockFactory();
    return pInstance;
}

StarBlockFactory::StarBlockFactory()
{
    first   = nullptr;
    last    = nullptr;
    nBlocks = 0;
    drawID  = 0;
    nCache  = DEFAULT_NCACHE;
}

StarBlockFactory::~StarBlockFactory()
{
    deleteBlocks(nBlocks);
    if (pInstance)
        pInstance = 0;
}

StarBlock *StarBlockFactory::getBlock()
{
    StarBlock *freeBlock = nullptr;

    if (nBlocks < nCache)
    {
        freeBlock = new StarBlock;
        if (freeBlock)
        {
            ++nBlocks;
            return freeBlock;
        }
    }
    if (last && (last->drawID != drawID || last->drawID == 0))
    {
        //        qDebug() << "Recycling block with drawID =" << last->drawID << "and current drawID =" << drawID;
        if (last->parent->block(last->parent->getBlockCount() - 1) != last)
            qDebug() << "ERROR: Goof up here!";
        freeBlock = last;
        last      = last->prev;
        if (last)
        {
            last->next = nullptr;
        }
        if (freeBlock == first)
        {
            first = nullptr;
        }
        freeBlock->reset();
        freeBlock->prev = nullptr;
        freeBlock->next = nullptr;
        return freeBlock;
    }
    freeBlock = new StarBlock;
    if (freeBlock)
        ++nBlocks;
    return freeBlock;
}

bool StarBlockFactory::markFirst(StarBlock *block)
{
    if (!block)
        return false;

    //    fprintf(stderr, "markFirst()!\n");
    if (!first)
    {
        //        qDebug() << "INFO: Linking in first block" << endl;
        last = first = block;
        first->prev = first->next = nullptr;
        first->drawID             = drawID;
        return true;
    }

    if (block == first) // Block is already in the front
    {
        block->drawID = drawID;
        return true;
    }

    if (block == last)
        last = block->prev;

    if (block->prev)
        block->prev->next = block->next;

    if (block->next)
        block->next->prev = block->prev;

    first->prev = block;
    block->next = first;
    block->prev = nullptr;
    first       = block;

    block->drawID = drawID;

    return true;
}

bool StarBlockFactory::markNext(StarBlock *after, StarBlock *block)
{
    //    fprintf(stderr, "markNext()!\n");
    if (!block || !after)
    {
        qDebug() << "WARNING: markNext called with nullptr argument" << endl;
        return false;
    }

    if (!first)
    {
        qDebug() << "WARNING: markNext called without an existing linked list" << endl;
        return false;
    }

    if (block == after)
    {
        qDebug() << "ERROR: Trying to mark a block after itself!" << endl;
        return false;
    }

    if (block->prev == after) // Block is already after 'after'
    {
        block->drawID = drawID;
        return true;
    }

    if (block == first)
    {
        if (block->next == nullptr)
        {
            qDebug() << "ERROR: Trying to mark only block after some other block";
            return false;
        }
        first = block->next;
    }

    if (after->getFaintMag() > block->getFaintMag() && block->getFaintMag() != -5)
    {
        qDebug() << "WARNING: Marking block with faint mag = " << block->getFaintMag() << " after block with faint mag "
                 << after->getFaintMag() << "in trixel" << block->parent->getTrixel();
    }

    if (block == last)
        last = block->prev;

    if (block->prev)
        block->prev->next = block->next;
    if (block->next)
        block->next->prev = block->prev;

    block->next = after->next;
    if (block->next)
        block->next->prev = block;
    block->prev = after;
    after->next = block;

    if (after == last)
        last = block;

    block->drawID = drawID;

    return true;
}

/*
bool StarBlockFactory::groupMove( StarBlock *start, const int nblocks ) {

    StarBlock * end = nullptr;

    // Check for trivial cases
    if( !start || nblocks < 0 )
        return false;

    if( nblocks == 0 )
        return true;

    if( !first )
        return false;

    // Check for premature end
    end = start;
    for( int i = 1; i < nblocks; ++i ) {
        if( end == nullptr )
            return false;
        end = end->next;
    }
    if( end == nullptr )
        return false;

    // Update drawIDs
    end = start;
    for( int i = 1; i < nblocks; ++i ) {
        end->drawID = drawID;
        end = end->next;
    }
    end->drawID = drawID;

    // Check if we are already in the front
    if( !start->prev )
        return true;

    start->prev->next = end->next;
    end->next->prev = start->prev;

    first->prev = end;
    end->next = first;
    start->prev = nullptr;
    first = start;
}
*/

int StarBlockFactory::deleteBlocks(int nblocks)
{
    int i           = 0;
    StarBlock *temp = nullptr;

    while (last != nullptr && i != nblocks)
    {
        temp = last->prev;
        delete last;
        last = temp;
        i++;
    }
    if (last)
        last->next = nullptr;
    else
        first = nullptr;

    qDebug() << nblocks << "StarBlocks freed from StarBlockFactory" << endl;

    nBlocks -= i;
    return i;
}

void StarBlockFactory::printStructure() const
{
    StarBlock *cur   = nullptr;
    Trixel curTrixel = 513; // TODO: Change if we change HTMesh level
    int index        = 0;
    bool draw        = false;

    cur = first;
    do
    {
        if (curTrixel != cur->parent->getTrixel())
        {
            qDebug() << "Trixel" << cur->parent->getTrixel() << "starts at index" << index << endl;
            curTrixel = cur->parent->getTrixel();
        }
        if (cur->drawID == drawID && !draw)
        {
            qDebug() << "Blocks from index" << index << "are drawn";
            draw = true;
        }
        if (cur->drawID != drawID && draw)
        {
            qDebug() << "Blocks from index" << index << "are not drawn";
            draw = false;
        }
        cur = cur->next;
        ++index;
    } while (cur != last);
}

int StarBlockFactory::freeUnused()
{
    int i           = 0;
    StarBlock *temp = nullptr;

    while (last != nullptr && last->drawID < drawID && i != nBlocks)
    {
        temp = last->prev;
        delete last;
        last = temp;
        i++;
    }
    if (last)
        last->next = nullptr;
    else
        first = nullptr;

    qDebug() << i << "StarBlocks freed from StarBlockFactory" << endl;

    nBlocks -= i;
    return i;
}
