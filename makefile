
CPPFLAGS += -I include -std=c++0x

src/test/test_% : src/test/%.cpp src/test/test_%.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@
