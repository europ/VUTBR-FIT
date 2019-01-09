require 'test/unit'
require_relative 'fib-sequence'

class FibonacciSequenceTest < Test::Unit::TestCase
    def setup
        @seq = FibonacciSequence.new
    end
        def test_reset
            assert_equal(true, @seq.reset) # (idx = -1)
            assert_equal(nil, @current_idx) # (idx = -1) => (idx < 0) => (return nil)
        end
        def test_current
            @seq.reset # (idx = -1)
            assert_equal(nil, @seq.current) # (idx = -1) => (seq[-1] = value nil)
            @seq.next # (idx += 1) => (idx = 0)
            assert_equal(0, @seq.current) # (idx = 0) => (seq[0] = value 0)
        end
        def test_current_idx
            @seq.reset # (idx = -1)
            assert_equal(nil, @seq.current_idx) # (idx = -1) => (idx < 0) => (return nil)
            @seq.next # (idx += 1) => (idx = 0)
            assert_equal(0, @seq.current_idx) # (idx = 0) => (idx >= 0) => (return idx = return 0)
        end
        def test_initialize
            @seq.reset # (idx = -1)
            assert_equal(nil, @seq.current) # (idx = -1) => (seq[-1] = value nil)
            assert_equal(nil, @seq.current_idx) # (idx = -1) => (idx < 0) => (return nil)
            @seq.next # (idx += 1) => (idx = 0)
            assert_equal(0, @seq.current) # (idx = 0) => (seq[0] = value 0)
            assert_equal(0, @seq.current_idx) # (idx = 0) => (idx >= 0) => (return idx = return 0)
            @seq.next # (idx += 1) => (idx = 1)
            assert_equal(1, @seq.current) # (idx = 1) => (seq[1] = value 1)
            assert_equal(1, @seq.current_idx) # (idx = 1) => (idx >= 0) => (return idx = return 1)
        end
        def test_next_01
            @seq.reset # (idx = -1)
            for i in 0..10
                @seq.next # 10x(idx += 1) => (idx = 10)
            end
            assert_equal(55, @seq.current) # (idx = 10) => (seq[10] = value 55)
            assert_equal(10, @seq.current_idx) # (idx = 10) => (idx >= 0) => (return idx = return 10)
        end
        def test_next_02
            @seq.reset # (idx = -1)
            for i in 0..20
                @seq.next # 20x(idx += 1) => (idx = 20)
            end
            assert_equal(6765, @seq.current) # (idx = 20) => (seq[20] = value 6765)
            assert_equal(20, @seq.current_idx) # (idx = 20) => (idx >= 0) => (return idx = return 20)
        end
        def test_n_negative
            @seq[-1]
            assert_equal(nil, @seq.current) # (idx = -1) => (seq[-1] = value nil)
            @seq[-10]
            assert_equal(nil, @seq.current) # (idx = -10) => (seq[-10] = value nil)
            @seq[-20]
            assert_equal(nil, @seq.current) # (idx = -20) => (seq[-20] = value nil)
        end
        def test_n_positive
            @seq[0]
            assert_equal(0, @seq.current) # (idx = 0) => (seq[0] = value 0)
            @seq[1]
            assert_equal(1, @seq.current) # (idx = 1) => (seq[1] = value 1)
            @seq[10]
            assert_equal(55, @seq.current) # (idx = 10) => (seq[10] = value 55)
            @seq[20]
            assert_equal(6765, @seq.current) # (idx = 20) => (seq[20] = value 6765)
        end
        def test_n_value_is_bigger_than_index
            @seq.reset # (idx = -1)
            for i in 0..8
                @seq.next # 8x(idx += 1) => (idx = 8)
            end
            assert_equal(21, @seq.current) # (idx = 8) => (seq[8] = value 21)
        end
        def test_n_value_is_smaller_than_index
            @seq.reset # (idx = -1)
            for i in 0..4
                @seq.next # 4x(idx += 1) => (idx = 4)
            end
            assert_equal(3, @seq.current) # (idx = 4) => (seq[4] = value 3)
        end
    # do testu budou zahrnuty metody s prefixem "test_", tj. napr. nasledujici
    def test_method
        assert_equal(true, true)
    end
end