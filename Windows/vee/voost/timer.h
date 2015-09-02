#ifndef _VEE_VOOST_TIMER_H_
#define _VEE_VOOST_TIMER_H_

namespace vee {
namespace voost {
namespace timer {

struct second_type
{
    unsigned int value;
};

struct millisecond_type
{
    unsigned int value;
};

inline second_type make_second(unsigned int duration)
{
    second_type ret;
    ret.value = duration;
    return ret;
}

inline millisecond_type make_millisecond(unsigned int duration)
{
    millisecond_type ret;
    ret.value = duration;
    return ret;
}

class async_timer abstract
{
public:
    using timer_tick = unsigned int;
    using delegate_t = delegate<void(timer_tick)>;
    virtual ~async_timer() = default;
    virtual bool run(const second_type time_period, const delegate_t& callback) = 0;
    virtual bool run(const millisecond_type time_period, const delegate_t& callback) = 0;
    virtual bool stop() = 0;
};

} // namespace timer
} // namespace voost
} // namespace vee

#endif // !_VEE_VOOST_TIMER_H_
