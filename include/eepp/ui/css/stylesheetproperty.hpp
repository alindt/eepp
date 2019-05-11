#ifndef EE_UI_CSS_STYLESHEETPROPERTY_HPP
#define EE_UI_CSS_STYLESHEETPROPERTY_HPP

#include <string>
#include <map>
#include <eepp/config.hpp>

namespace EE { namespace UI { namespace CSS {

class EE_API StyleSheetProperty {
	public:
		StyleSheetProperty();

		explicit StyleSheetProperty( const std::string& name, const std::string& value );

		explicit StyleSheetProperty( const std::string& name, const std::string& value, const Uint32& specificity );

		const std::string& getName() const;

		const std::string& getValue() const;

		const Uint32& getSpecificity() const;

		void setSpecificity( const Uint32& specificity );

		bool isEmpty() const;

		void setName(const std::string & name);

		void setValue(const std::string & value);
	protected:
		std::string mName;
		std::string mValue;
		Uint32 mSpecificity;
};

typedef std::map<std::string, StyleSheetProperty> StyleSheetProperties;

}}}

#endif