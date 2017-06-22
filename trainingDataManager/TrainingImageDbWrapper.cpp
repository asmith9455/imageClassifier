#include "TrainingImageDbWrapper.h"

TrainingImageDbWrapper::TrainingImageDbWrapper()
{

}


std::vector<ImageSequence> TrainingImageDbWrapper::getTilesWithSingleProperty(QString dbFilePath, int propertyID)
{
    std::vector<SegmentedImage<int>> segImgs
            = TrainingImageDbWrapper::getSegmentedImagesWithSingleProperty(dbFilePath, propertyID);

    //go through each segmented region in segImgs and figure out all the property combinations we will use to create image sequences

    //create the image sequences to return
    std::vector<ImageSequence> imseqs;
    return imseqs;
}

std::vector<SegmentedImage<int>> TrainingImageDbWrapper::getSegmentedImagesWithSingleProperty(QString dbFilePath, int propertyID)
{
    std::vector<SegmentedImage<int>> segImgs;

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(dbFilePath);

    if (!database.open())
    {
        throw runtime_error("Error - Could not open a connection to the database.");
    }
    //enable foreign keys
    database.exec("PRAGMA foreign_keys = ON;");





    //get a list of all segmented region ids have the desired property using the regionProperties table
    QSqlQuery query;
    //note: due to unique constraint uniqueProperties in the DB - the same segregion cannot have 2 of the same properties
    query.prepare("SELECT segmentedRegionReference FROM regionProperties WHERE propertyReference = :W_id;");
    query.bindValue(":W_id", propertyID);


    if(!query.exec())
    {
        database.close();
        throw runtime_error("Error - could not query the segmented regions for the given property reference id"
                +std::to_string(propertyID)+". Error is: " + query.lastError().text().toStdString());
    }

    std::vector<SegmentedRegion<int>> segRegions;

    //for each segmented region returned,
    while( query.next() )
    {
        int segRegionID = query.value(0).toInt();
        //we need to add this segRegion to a
        SegmentedRegion<int> sr;
        sr.segmentedRegionID = segRegionID;

        //get all of the other
        QSqlQuery query2;
        query2.prepare("SELECT propertyReference FROM regionProperties WHERE segmentedRegionReference = :W_id;");
        query2.bindValue(":W_id", propertyID);


        if(!query2.exec())
        {
            database.close();
            throw runtime_error("Error: " + query2.lastError().text().toStdString());
        }

        while( query.next() )
        {
            sr.propertyIDs.push_back(query.value(0).toInt());
        }

        segRegions.push_back(sr);
    }

    //now segRegions contains all segmented regions that are associeted with propertyID
    //also, all associated property ids for each segmented region is included

    //get all of the segmented regions and their corresponding image reference ids
    for (size_t i = 0; i < segRegions.size(); i++)
    {
        query.prepare("SELECT imageReference, outerContour, innerContours FROM segmentedRegions WHERE id = :W_id;");
        query.bindValue(":W_id", segRegions[i].segmentedRegionID);


        if(!query.exec())
        {
            database.close();
            throw runtime_error("Error: " + query.lastError().text().toStdString());
        }

        while( query.next() )
        {
            segRegions[i].imgID = query.value(0).toInt();
            //segRegions[i]. = query.value(0).toInt();
        }
    }


    //get all of the images, and create SegmentedImage classes to return

    return segImgs;
}
