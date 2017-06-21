#ifndef TRAININGIMAGEDBWRAPPER_H
#define TRAININGIMAGEDBWRAPPER_H

#include <ImageSequence.h>
#include <SegmentedImage.h>

class TrainingImageDbWrapper
{
public:
    TrainingImageDbWrapper();

    static std::vector<ImageSequence> getTilesForProperty(std::string dbFilePath);

    static std::vector<SegmentedImage<int>> getSegmentedImagesByProperty(std::string propertySelectionSqlCriteria);
};

#endif // TRAININGIMAGEDBWRAPPER_H
