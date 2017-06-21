#include "TrainingImageDbWrapper.h"

TrainingImageDbWrapper::TrainingImageDbWrapper()
{

}

std::vector<ImageSequence> TrainingImageDbWrapper::getTilesForProperty(std::string dbFilePath, std::string targetProperty)
{
    //go through each segmented region and figure out all the property combinations that occur with the indicated property
    return NULL;
}

std::vector<SegmentedImage<int>> TrainingImageDbWrapper::getSegmentedImagesByProperty(std::string propertySelectionSqlCriteria)
{
    std::vector<SegmentedImage<int>> segImgs;

    //get a list of all segmented region ids that fit the property selection criteria using the regionProperties table

    //get all of the segmented regions and their corresponding image reference ids

    //get all of the images, and create SegmentedImage classes to return

    return segImgs;
}
