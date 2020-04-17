#include "ThetisTest.h"

using namespace rapidxml;

// Not a real test but a ~ quick demo
TEST_THETIS(TestXml01) {
    auto folder = thetis::executable_path();
    spdlog::debug("executable path = {}", folder);
    auto path = folder 
        + std::string(1, thetis::kPathSeparator) 
        + ".."
        + std::string(1, thetis::kPathSeparator) 
        + "testData"
        + std::string(1, thetis::kPathSeparator) 
        + "beerJournal.xml";
    spdlog::debug("xml path = {}", path);
    std::ifstream theFile(path);

	xml_document<> doc;
	xml_node<> * root_node;
	std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc 
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("MyBeerJournal");
	// Iterate over the brewerys
	for (xml_node<> * brewery_node = root_node->first_node("Brewery"); brewery_node; brewery_node = brewery_node->next_sibling())
	{
	    printf("I have visited %s in %s. ", 
	    	brewery_node->first_attribute("name")->value(),
	    	brewery_node->first_attribute("location")->value());
            // Interate over the beers
	    for(xml_node<> * beer_node = brewery_node->first_node("Beer"); beer_node; beer_node = beer_node->next_sibling())
	    {
	    	printf("On %s, I tried their %s which is a %s. ", 
	    		beer_node->first_attribute("dateSampled")->value(),
	    		beer_node->first_attribute("name")->value(), 
	    		beer_node->first_attribute("description")->value());
	    	printf("I gave it the following review: %s", beer_node->value());
	    }
	    std::cout << std::endl;
	}        
}

