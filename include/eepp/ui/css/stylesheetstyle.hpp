#ifndef EE_UI_CSS_STYLESHEETSTYLE_HPP
#define EE_UI_CSS_STYLESHEETSTYLE_HPP

#include <eepp/ui/css/stylesheetselector.hpp>
#include <eepp/ui/css/stylesheetproperty.hpp>

namespace EE { namespace UI { namespace CSS {

class EE_API StyleSheetStyle {
	public:
		StyleSheetStyle();

		explicit StyleSheetStyle( const std::string& selector, const StyleSheetProperties& properties );

		void print();

		const StyleSheetSelector& getSelector() const;

		const StyleSheetProperties& getProperties() const;

		void setProperty( const StyleSheetProperty& property );
	protected:
		StyleSheetSelector mSelector;
		StyleSheetProperties mProperties;
};

class EE_API StyleSheetStyleUsed {
	public:
		StyleSheetStyle style;
		bool used;

		StyleSheetStyleUsed( StyleSheetStyle style, bool used ) :
			style( style ),
			used( used )
		{}
};

typedef std::map<std::string, StyleSheetStyle> StyleSheetStyleList;
typedef std::vector<StyleSheetStyle> StyleSheetStyleVector;
typedef std::vector<StyleSheetStyleUsed> StyleSheetStyleUsedVector;

}}}

#endif