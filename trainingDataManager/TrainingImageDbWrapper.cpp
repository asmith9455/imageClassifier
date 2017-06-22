#include "TrainingImageDbWrapper.h"

TrainingImageDbWrapper::TrainingImageDbWrapper()
{

}

//for imp
/*std::size_t operator()(std::vector<int> const& vec) const {
  std::size_t seed = vec.size();
  for(auto& i : vec) {
    seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  return seed;
}*/

std::vector<ImageSequence> TrainingImageDbWrapper::getTilesWithSingleProperty
(QString dbFilePath, int captureDeviceID, int propertyID , int tileWidth, int tileHeight)
{
    std::vector<SegmentedImage<int>> segImgs
            = TrainingImageDbWrapper::getSegmentedImagesWithSingleProperty(dbFilePath, captureDeviceID, propertyID);

    //go through each segmented region in segImgs and figure out all unique property combinations we will use to create image sequences
    //map lists of property ids to lists of indices in the segImgs vector
    std::map<std::vector<int>,
            std::vector<std::pair<size_t, size_t>>>
            combos;

    bool inMap;

    for (size_t i = 0; i < segImgs.size(); i++)
        for (size_t j = 0; j < segImgs[i].segmentedRegions.size(); j++)
        {
            auto mapPair = combos.find(segImgs[i].segmentedRegions[j].propertyIDs);
            inMap = mapPair != combos.end();

            if (inMap)
                mapPair->second.push_back(std::make_pair(i,j));// / txtLenDbl);
            else
            {
                std::vector<std::pair<size_t, size_t>> tmp = {std::make_pair(i,j)};
                combos.insert(
                        std::make_pair(
                                segImgs[i].segmentedRegions[j].propertyIDs,
                                tmp)
                            );
            }
        }

    //create the image sequences to return
    std::vector<ImageSequence> imseqs;

    //combos has all of the indices for each unique combination of properties
    //these will be our pre-clustered image sequences
    for (auto it : combos)
    {
        ImageSequence iseq;
        for (std::pair<size_t, size_t> indexPair : it.second)
            if (segImgs[indexPair.first].segmentedRegions[indexPair.second].hasProperty(propertyID))
                iseq.addImgs(
                    ConverterMethods::
                            getImageSequenceFromSegmentedRegion(
                                segImgs[indexPair.first].img, segImgs[indexPair.first].segmentedRegions[indexPair.second], tileWidth, tileHeight));
            else //note that the sql queries should guarantee that each seg region has the desired property
                throw runtime_error("There is an error in the sql select logic. All the segmented regions should have the targetPropertyID.");
        imseqs.push_back(iseq);
    }

    return imseqs;
}

std::vector<SegmentedImage<int>> TrainingImageDbWrapper::getSegmentedImagesWithSingleProperty(QString dbFilePath, int captureDeviceID, int propertyID)
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
        query2.bindValue(":W_id", segRegionID);


        if(!query2.exec())
        {
            database.close();
            throw runtime_error("Error: " + query2.lastError().text().toStdString());
        }

        while( query2.next() )
        {
            sr.propertyIDs.push_back(query2.value(0).toInt());
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
            segRegions[i].outerContour
                    = ConverterMethods::getContourClassFromString(query.value(1).toString());
            segRegions[i].innerContours
                    = ConverterMethods::getContourClassVectorFromString(query.value(2).toString());
        }
    }

    //now we have a list of segmented regions, some of which may be from a common image.
    //the next step is to combine segmented regions that share a common image.
    //TODO: change this to use an unordered map

    std::unordered_map<int, std::vector<size_t>> img2rgn;

    bool inMap;


    for (size_t j = 0; j < segRegions.size(); j++)
    {
        auto mapPair = img2rgn.find(segRegions[j].imgID);
        inMap = mapPair != img2rgn.end();

        if (inMap)
            mapPair->second.push_back(j);// / txtLenDbl);
        else
        {
            std::vector<size_t> tmp = {j};
            img2rgn.insert(
                    std::make_pair(segRegions[j].imgID, tmp)
                        );
        }
    }

    for (auto it : img2rgn)
    {
        SegmentedImage<int> si;
        for (size_t regInd : it.second)
            si.segmentedRegions.push_back(segRegions[regInd]);
        si.imgID = it.first;
        segImgs.push_back(si);
    }




    //now just need to fetch the image data for each image
    for (size_t i = segImgs.size()-1; i >=0; i--)
    {
        query.prepare("SELECT captureDevice, image FROM images WHERE id = :W_id;");
        query.bindValue(":W_id", segImgs[i].imgID);

        if(!query.exec())
        {
            database.close();
            throw runtime_error("Error: " + query.lastError().text().toStdString());
        }

        if( query.next() )
        {
            int capDevID = query.value(0).toInt();
            segImgs[i].captureDeviceID = capDevID;
            if (capDevID == captureDeviceID)
            {
                segImgs[i].img
                        = CvQt::qbytearray_2_mat(query.value(1).toByteArray());
                //cv::imshow("test",segImgs[i].img);
                //cv::waitKey(0);
                //cv::destroyAllWindows();
            }
            else
                segImgs.erase(segImgs.begin() + i);
        }
        //necessary because otherwise i will loop back around to the top of the range of size_t
        if (i == 0)
            break;
    }

    return segImgs;
}
