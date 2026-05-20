#include <cmath>

class DataBlocking
{
    unsigned int value_counter = 0;
    unsigned int block_counter = 0;

    double block_sum = 0;
    double last_block_ave;

    double running_ave = 0;  // cumulative mean of block averages
    double running_ave2 = 0; // cumulative mean of squared block averages

public:
    DataBlocking() = default;

    void add(const double value)
    {
        block_sum += value;
        value_counter++;
    }
    void reset_block()
    {
        if (value_counter == 0)
            return;

        double block_ave = block_sum / static_cast<double>(value_counter);

        block_counter++;
        running_ave += (block_ave - running_ave) / static_cast<double>(block_counter);
        running_ave2 += (block_ave * block_ave - running_ave2) / static_cast<double>(block_counter);

        last_block_ave = block_ave;

        block_sum = 0;
        value_counter = 0;
    }

    double get_ave() const { return running_ave; }
    double get_ave2() const { return running_ave2; }
    double get_error() const
    {
        if (block_counter <= 1)
            return 0.;
        else
            return std::sqrt(std::fabs(running_ave2 - running_ave * running_ave) / static_cast<double>(block_counter - 1));
    }
    double get_ave_blk() const { return last_block_ave; }
};
