#include "EvaClientOAuth.h"

class EvaClientHttp : public EvaClientOAuth {
    
    public:
    
        void postData(
            const String feed, 
            const String source, 
            const String datapoints);
            
        void postData(
            const String feed, 
            const String source, 
            JsonArray& datapoints);

};
