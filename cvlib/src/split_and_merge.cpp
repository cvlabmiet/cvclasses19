/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-05
 * @author Anonymous
 */

#include "cvlib.hpp"
#include <vector>

struct Tree
{
    std::vector<Tree> childs;
    int x1, y1, x2, y2;
    bool leaf;

    Tree(int x1_ = 0, int y1_ = 0, int x2_ = 0, int y2_ = 0)
        : x1(x1_), y1(y1_), x2(x2_), y2(y2_), leaf(true) {}
};


namespace
{
    void split_image(cv::Mat image, double stddev, Tree *tree)
    {
            
        cv::Mat mean;
        cv::Mat dev;
        cv::meanStdDev(image, mean, dev);

        if (dev.at<double>(0) <= stddev)
        {
            tree->leaf = true;
            return;
        }

        const auto width = image.cols;
        const auto height = image.rows;


        if ((height > 1) && (width > 1))
        {
            tree->leaf = false;

            int dy = (tree->y2 - tree->y1) / 2;
            int dx = (tree->x2 - tree->x1) / 2;
            

            /*
                1 | 2
                -----
                3 | 4
            */

            tree->childs.push_back(Tree(tree->x1, tree->y1, tree->x1 + dx, tree->y1 + dy));

            tree->childs.push_back(Tree(tree->x1 + dx, tree->y1, tree->x2, tree->y1 + dy));

            tree->childs.push_back(Tree(tree->x1, tree->y1 + dy, tree->x1 + dx, tree->y2));

            tree->childs.push_back(Tree(tree->x1 + dx, tree->y1 + dy, tree->x2, tree->y2));

            split_image(image(cv::Range(0, height / 2), cv::Range(0, width / 2)), stddev, &(tree->childs[0]));
            split_image(image(cv::Range(0, height / 2), cv::Range(width / 2, width)), stddev, &(tree->childs[1]));
            split_image(image(cv::Range(height / 2, height), cv::Range(width / 2, width)), stddev, &(tree->childs[2]));
            split_image(image(cv::Range(height / 2, height), cv::Range(0, width / 2)), stddev, &(tree->childs[3]));
        }
    }

} // namespace

bool merge_leafs(cv::Mat& image, double stddev, Tree* tree1, Tree* tree2)
{
	int y1 = std::min(tree1->y1, tree2->y1);
    int y2 = std::max(tree1->y2, tree2->y2);
    int x1 = std::min(tree1->x1, tree2->x1);
    int x2 = std::max(tree1->x2, tree2->x2);
    
    cv::Mat combined = image(cv::Range(y1, y2), cv::Range(x1, x2));
    if (combined.empty()) return false;
    
    cv::Mat mean, dev;
    cv::meanStdDev(combined, mean, dev);
    
    if (dev.at<double>(0) <= stddev)
    {
        combined.setTo(mean);
        return true;
    }

    return false;
}

bool are_neighbors(Tree* a, Tree* b)
{
    bool horizontal = (a->x2 == b->x1 || b->x2 == a->x1) &&
                      std::max(a->y1, b->y1) < std::min(a->y2, b->y2);
    
    bool vertical = (a->y2 == b->y1 || b->y2 == a->y1) &&
                    std::max(a->x1, b->x1) < std::min(a->x2, b->x2);
    
    return horizontal || vertical;
}

void merge_image(cv::Mat& image, double stddev, Tree* tree)
{
    if (tree->leaf) return;
    if (!tree->childs.empty())
    {
        for (auto& child : tree->childs)
            merge_image(image, stddev, &child);
        
        bool merged;
        do {
            merged = false;
            for (size_t i = 0; i < tree->childs.size() && !merged; ++i)
            {
                for (size_t j = i + 1; j < tree->childs.size(); ++j)
                {
                    if (tree->childs[i].leaf && tree->childs[j].leaf)
                    {
                        if (are_neighbors(&tree->childs[i], &tree->childs[j]))
                        {
                            if (merge_leafs(image, stddev, &tree->childs[i], &tree->childs[j]))
                            {
                                int y1 = std::min(tree->childs[i].y1, tree->childs[j].y1);
                                int y2 = std::max(tree->childs[i].y2, tree->childs[j].y2);
                                int x1 = std::min(tree->childs[i].x1, tree->childs[j].x1);
                                int x2 = std::max(tree->childs[i].x2, tree->childs[j].x2);
                                
                                tree->childs.erase(tree->childs.begin() + j);
                                
                                tree->childs[i].y1 = y1;
                                tree->childs[i].y2 = y2;
                                tree->childs[i].x1 = x1;
                                tree->childs[i].x2 = x2;
                                
                                merged = true;
                                break;
                            }
                        }
                    }
                }
            }
        } while (merged && tree->childs.size() >= 2);
    }
}


namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev)
{
    // split part
    cv::Mat res = image;
    Tree tree(0, 0, image.cols, image.rows);
    split_image(res, stddev, &tree);

    // merge part
    merge_image(res, stddev, &tree);
    return res;
}
} // namespace cvlib


