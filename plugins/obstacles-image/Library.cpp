/* ************************************************************************ */
/* Georgiev Lab (c) 2015                                                    */
/* ************************************************************************ */
/* Department of Cybernetics                                                */
/* Faculty of Applied Sciences                                              */
/* University of West Bohemia in Pilsen                                     */
/* ************************************************************************ */

// OpenCV
#include <opencv2/opencv.hpp>

// Simulator
#include "core/DynamicArray.hpp"
#include "simulator/Plugin.hpp"
#include "simulator/PluginApi.hpp"
#include "simulator/Simulation.hpp"
#include "simulator/Configuration.hpp"
#include "simulator/Obstacle.hpp"

/* ************************************************************************ */

using namespace simulator;

/* ************************************************************************ */

class ObstaclesImageApi : public PluginApi
{
    void configure(Simulation& simulation, const Configuration& config) override
    {
        static constexpr int THRESHOLD = 127;

        using namespace cv;

        // Get image file path
        const auto path = config.buildFilePath(config.get("image"));

        // Load obstacle image
        Mat img = imread(path.string());

        if (img.empty())
            throw InvalidArgumentException("Cannot open source image: " + path.string());

        // Flip image
        flip(img, img, 0);

        // Remove colors
        cvtColor(img, img, CV_BGR2GRAY);

        // To Black & White
        img = img > THRESHOLD;

        // Detect contours
        {
            // Get contours
            DynamicArray<DynamicArray<Point>> contoursSrc;
            DynamicArray<Vec4i> hierarchy;
            findContours(img, contoursSrc, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

            // For each contour compute aproximate version
            DynamicArray<DynamicArray<Point>> contours(contoursSrc.size());

            for (std::size_t i = 0u; i < contoursSrc.size(); ++i)
            {
                approxPolyDP(contoursSrc[i], contours[i], 1, true);
            }

            const auto size = core::Vector<unsigned>(img.size().width, img.size().height);

            // Coordinate scale
            const auto scale = simulation.getWorldSize() / size;
            const auto off = size * 0.5;

            // Create obstacle body
            auto obstacle = simulation.createObject<simulator::Obstacle>();
            auto& shapes = obstacle->getMutableShapes();
            shapes.reserve(contours.size());

            for (const auto& cnt : contours)
            {
                simulator::ShapeEdges shape;

                for (std::size_t i = 0u; i < cnt.size(); ++i)
                {
                    const core::Vector<float> p(cnt[i].x, cnt[i].y);
                    shape.edges.push_back((p - off) * scale);
                }

                shapes.push_back(std::move(shape));
            }

            obstacle->initShapes();
        }
    }
};

/* ************************************************************************ */

DEFINE_PLUGIN(obstacles_image, ObstaclesImageApi)

/* ************************************************************************ */