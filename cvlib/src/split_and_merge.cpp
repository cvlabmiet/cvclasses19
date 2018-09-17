/* Split and merge segmentation algorithm implementation.
 * @file
 * @date 2018-09-10
 * @author Max Kimlyk
 */

#include "cvlib.hpp"
#include "utils.hpp"

#include <list>

namespace
{
enum // Direction
{
    LEFT = 1,
    RIGHT = -1,
    UP = 2,
    DOWN = -2
};
typedef int Direction;

struct Edge;
struct Node;
typedef std::list<Node>::iterator node_iterator;

struct Edge
{
    node_iterator node;
    Direction direction;

    Edge(node_iterator node, Direction direction = LEFT) : node(node), direction(direction)
    {
    }

    bool operator==(const Edge& another) const
    {
        return node == another.node;
    }
};

struct Node
{
    std::list<cv::Rect> rects;
    double sum;
    double sum_sq;
    int area;

    std::list<Edge> neighbours;

    Node(const cv::Rect& rect)
    {
        rects.push_back(rect);
    }

    void add_neighbour(node_iterator node, Direction direction)
    {
        auto iter = std::find(neighbours.begin(), neighbours.end(), Edge(node));
        if (iter == neighbours.end())
            neighbours.emplace_back(Edge(node, direction));
        else
            iter->direction = direction;
    }

    void remove_neighbour(node_iterator node)
    {
        auto iter = std::find(neighbours.begin(), neighbours.end(), Edge(node));
        if (iter != neighbours.end())
            neighbours.erase(iter);
    }
};

class SplitMergeGraph
{
    private:
    const cv::Mat& image_;
    double disp_thresh_;

    cv::Mat integral_;
    cv::Mat integral_sq_;
    std::list<Node> nodes_;

    void make_neighbours(node_iterator node1, node_iterator node2, Direction from1to2)
    {
        node1->add_neighbour(node2, from1to2);
        node2->add_neighbour(node1, Direction(-from1to2));
    }

    std::tuple<node_iterator, node_iterator, node_iterator, node_iterator> split_node(node_iterator node, const cv::Rect& rect_tl, const cv::Rect& rect_tr,
                                                                                      const cv::Rect& rect_bl, const cv::Rect& rect_br)
    {
        node_iterator node1 = nodes_.insert(nodes_.end(), Node(rect_tl));
        node_iterator node2 = nodes_.insert(nodes_.end(), Node(rect_tr));
        node_iterator node3 = nodes_.insert(nodes_.end(), Node(rect_bl));
        node_iterator node4 = nodes_.insert(nodes_.end(), Node(rect_br));

        make_neighbours(node1, node2, RIGHT);
        make_neighbours(node2, node4, DOWN);
        make_neighbours(node4, node3, LEFT);
        make_neighbours(node3, node1, UP);

        for (auto edge = node->neighbours.begin(); edge != node->neighbours.end(); ++edge)
        {
            edge->node->remove_neighbour(node);

            switch (edge->direction)
            {
                case LEFT:
                    make_neighbours(node1, edge->node, LEFT);
                    make_neighbours(node3, edge->node, LEFT);
                    break;

                case RIGHT:
                    make_neighbours(node2, edge->node, RIGHT);
                    make_neighbours(node4, edge->node, RIGHT);
                    break;

                case UP:
                    make_neighbours(node1, edge->node, UP);
                    make_neighbours(node2, edge->node, UP);
                    break;

                case DOWN:
                    make_neighbours(node2, edge->node, DOWN);
                    make_neighbours(node4, edge->node, DOWN);
                    break;
            }
        }

        nodes_.erase(node);

        return std::make_tuple(node1, node2, node3, node4);
    }

    void preprocess_merge()
    {
        for (auto& node : nodes_)
        {
            node.sum = cvlib_utils::sum_in_rect_by_integral<int>(integral_, node.rects.front());
            node.sum_sq = cvlib_utils::sum_in_rect_by_integral<double>(integral_sq_, node.rects.front());
            node.area = node.rects.front().area();
        }
    }

    void merge_nodes(node_iterator first, node_iterator second)
    {
        for (auto& neighbour : second->neighbours)
        {
            if (neighbour.node != first)
            {
                neighbour.node->remove_neighbour(second);
                first->add_neighbour(neighbour.node, neighbour.direction);
                neighbour.node->add_neighbour(first, -neighbour.direction);
            }
        }

        first->remove_neighbour(second);

        first->rects.insert(first->rects.end(), second->rects.begin(), second->rects.end());

        nodes_.erase(second);
    }

    public:
    SplitMergeGraph(const cv::Mat& image, double std_thresh = 1.0) : image_(image), disp_thresh_(std_thresh * std_thresh)
    {
        cv::integral(image, integral_, integral_sq_);
    }

    void split(int min_chunk_size)
    {
        nodes_.clear();
        nodes_.emplace_back(Node(cv::Rect(0, 0, image_.cols, image_.rows)));
        split(nodes_.begin(), min_chunk_size);
    }

    void split(node_iterator current_node, int min_chunk_size)
    {
        cv::Rect rect = current_node->rects.front();
        if (rect.width <= min_chunk_size || rect.height <= min_chunk_size)
            return;

        double mean = 0.0;
        double disp = 0.0;
        cvlib_utils::mean_disp_by_integral(integral_, integral_sq_, rect, mean, disp);

        if (disp <= disp_thresh_)
            return;

        int new_width1 = rect.width / 2;
        int new_height1 = rect.height / 2;
        int new_width2 = rect.width - new_width1;
        int new_height2 = rect.height - new_height1;

        cv::Rect rect1(rect.x, rect.y, new_width1, new_height1);
        cv::Rect rect2(rect.x + new_width1, rect.y, new_width2, new_height1);
        cv::Rect rect3(rect.x, rect.y + new_height1, new_width1, new_height2);
        cv::Rect rect4(rect.x + new_width1, rect.y + new_height1, new_width2, new_height2);

        auto new_nodes = split_node(current_node, rect1, rect2, rect3, rect4);

        split(std::get<0>(new_nodes), min_chunk_size);
        split(std::get<1>(new_nodes), min_chunk_size);
        split(std::get<2>(new_nodes), min_chunk_size);
        split(std::get<3>(new_nodes), min_chunk_size);
    }

    void merge()
    {
        preprocess_merge();

        for (auto node = nodes_.begin(); node != nodes_.end(); ++node)
            merge_with_neighbours(node);
    }

    void merge_slow()
    {
        preprocess_merge();

        bool is_processing = true;
        while (is_processing)
        {
            is_processing = false;

            for (auto node = nodes_.begin(); node != nodes_.end(); ++node)
            {
                bool was_merges = merge_with_neighbours(node);
                is_processing = is_processing || was_merges;
            }
        }
    }

    bool merge_with_neighbours(node_iterator node)
    {
        bool was_merges = false;

        auto next = node->neighbours.begin();
        for (auto neighbour = next; neighbour != node->neighbours.end(); neighbour = next)
        {
            ++next;

            // чтобы не обрабатывать одинаковые пары нод дважды, будем мержить только в направлении вправо и вниз
            if (neighbour->direction == LEFT || neighbour->direction == UP)
                continue;

            double new_sum = node->sum + neighbour->node->sum;
            double new_sum_sq = node->sum_sq + neighbour->node->sum_sq;
            int new_area = node->area + neighbour->node->area;

            double new_mean = new_sum / new_area;
            double new_disp = new_sum_sq / new_area - new_mean * new_mean;

            if (new_disp <= disp_thresh_)
            {
                merge_nodes(node, neighbour->node);
                node->sum = new_sum;
                node->sum_sq = new_sum_sq;
                node->area = new_area;
                was_merges = true;
            }
        }

        return was_merges;
    }

    cv::Mat draw_segments()
    {
        cv::Mat result(image_.rows, image_.cols, CV_8UC1);
        for (auto& node : nodes_)
        {
            for (auto rect : node.rects)
            {
                double d = double(node.sum) / double(node.area);
                uint8_t val = uint8_t(d);
                result(rect).setTo(val);
            }
        }
        return result;
    }

    cv::Mat draw_segments_colored(cv::Mat image)
    {
        cv::Mat result(image.rows, image.cols, CV_8UC3);
        for (auto& node : nodes_)
        {
            cv::Scalar acc = 0;
            for (auto rect : node.rects)
            {
                cv::Scalar sum = cv::sum(image);
                acc += cv::sum(image(rect));
            }
            cv::Scalar mean = acc / (double)(node.area);
            for (auto rect : node.rects)
                result(rect).setTo(mean);
        }
        return result;
    }
};

cv::Mat make_grayscale(const cv::Mat& input)
{
    cv::Mat gray;
    cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    return gray;
}
} // namespace

namespace cvlib
{
cv::Mat split_and_merge(const cv::Mat& image, double stddev, int min_chunk_size)
{
    bool colored_input = image.channels() != 1;
    const cv::Mat& image_gray = colored_input ? make_grayscale(image) : image;

    SplitMergeGraph graph(image_gray, stddev);

    graph.split(min_chunk_size);
    graph.merge();

    if (colored_input)
        return graph.draw_segments_colored(image);
    else
        return graph.draw_segments();
}
} // namespace cvlib
