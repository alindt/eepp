#include "pluginmanager.hpp"
#include <eepp/ui/uicheckbox.hpp>
#include <eepp/ui/uitableview.hpp>
#include <eepp/ui/uiwidgetcreator.hpp>

namespace ecode {

PluginManager::PluginManager( const std::string& resourcesPath, const std::string& pluginsPath,
							  std::shared_ptr<ThreadPool> pool ) :
	mResourcesPath( resourcesPath ), mPluginsPath( pluginsPath ), mThreadPool( pool ) {}

PluginManager::~PluginManager() {
	for ( auto& plugin : mPlugins )
		eeDelete( plugin.second );
}

void PluginManager::registerPlugin( const PluginDefinition& def ) {
	mDefinitions[def.id] = def;
}

UICodeEditorPlugin* ecode::PluginManager::get( const std::string& id ) {
	auto findIt = mPlugins.find( id );
	if ( findIt != mPlugins.end() )
		return findIt->second;
	return nullptr;
}

bool PluginManager::setEnabled( const std::string& id, bool enable ) {
	mPluginsEnabled[id] = enable;
	UICodeEditorPlugin* plugin = get( id );
	if ( enable && plugin == nullptr && hasDefinition( id ) ) {
		UICodeEditorPlugin* newPlugin = mDefinitions[id].creatorFn( this );
		mPlugins.insert( std::pair<std::string, UICodeEditorPlugin*>( id, newPlugin ) );
		if ( onPluginEnabled )
			onPluginEnabled( newPlugin );
		return true;
	}
	if ( !enable && plugin != nullptr ) {
		eeSAFE_DELETE( plugin );
		mPlugins.erase( id );
	}
	return false;
}

bool PluginManager::isEnabled( const std::string& id ) const {
	return mPluginsEnabled.find( id ) != mPluginsEnabled.end() ? mPluginsEnabled.at( id ) : false;
}

const std::string& PluginManager::getResourcesPath() const {
	return mResourcesPath;
}

const std::string& PluginManager::getPluginsPath() const {
	return mPluginsPath;
}

const std::map<std::string, bool>& PluginManager::getPluginsEnabled() const {
	return mPluginsEnabled;
}

void PluginManager::onNewEditor( UICodeEditor* editor ) {
	for ( auto& plugin : mPlugins )
		editor->registerPlugin( plugin.second );
}

void PluginManager::setPluginsEnabled( const std::map<std::string, bool>& pluginsEnabled ) {
	mPluginsEnabled = pluginsEnabled;
	for ( const auto& plugin : pluginsEnabled ) {
		if ( plugin.second && get( plugin.first ) == nullptr )
			setEnabled( plugin.first, true );
	}
}

const std::shared_ptr<ThreadPool>& PluginManager::getThreadPool() const {
	return mThreadPool;
}

const std::map<std::string, PluginDefinition>& PluginManager::getDefinitions() const {
	return mDefinitions;
}

const PluginDefinition* PluginManager::getDefinitionIndex( const Int64& index ) const {
	const PluginDefinition* def = nullptr;
	Int64 i = 0;
	for ( const auto& curDef : mDefinitions ) {
		if ( index == i )
			def = &curDef.second;
		++i;
	}
	return def;
}

bool PluginManager::hasDefinition( const std::string& id ) {
	return mDefinitions.find( id ) != mDefinitions.end();
}

std::shared_ptr<PluginsModel> PluginsModel::New( PluginManager* manager ) {
	return std::make_shared<PluginsModel>( manager );
}

size_t PluginsModel::rowCount( const ModelIndex& ) const {
	return mManager->getDefinitions().size();
}

std::string PluginsModel::columnName( const size_t& col ) const {
	eeASSERT( col < mColumnNames.size() );
	return mColumnNames[col];
}

Variant PluginsModel::data( const ModelIndex& index, ModelRole role ) const {
	if ( role == ModelRole::Display ) {
		const PluginDefinition* def = mManager->getDefinitionIndex( index.row() );
		if ( def == nullptr )
			return {};
		switch ( index.column() ) {
			case Columns::Version:
				return Variant( def->versionString.c_str() );
			case Columns::Description:
				return Variant( def->description.c_str() );
			case Columns::Title:
				return Variant( def->name.c_str() );
			case Columns::Enabled:
				return Variant( mManager->isEnabled( def->id ) );
			case Columns::Id:
				return Variant( def->id.c_str() );
		}
	}
	return {};
}

PluginManager* PluginsModel::getManager() const {
	return mManager;
}

class UIPluginManagerTable : public UITableView {
  public:
	UIPluginManagerTable() : UITableView() {}

	std::function<UITextView*( UIPushButton* )> getCheckBoxFn( const ModelIndex& index,
															   const PluginsModel* model ) {
		return [index, model]( UIPushButton* but ) -> UITextView* {
			UICheckBox* chk = UICheckBox::New();
			chk->setChecked(
				model->data( model->index( index.row(), PluginsModel::Enabled ) ).asBool() );
			but->addEventListener( Event::MouseClick, [&, index, model, chk]( const Event* event ) {
				if ( !( event->asMouseEvent()->getFlags() & EE_BUTTON_LMASK ) )
					return 1;
				UIWidget* chkBut = chk->getCurrentButton();
				auto mousePos =
					chkBut->convertToNodeSpace( event->asMouseEvent()->getPosition().asFloat() );
				if ( chkBut->getLocalBounds().contains( mousePos ) ) {
					bool checked = !chk->isChecked();
					chk->setChecked( checked );
					std::string id(
						model->data( model->index( index.row(), PluginsModel::Id ) ).asCStr() );
					model->getManager()->setEnabled( id, checked );
				}
				return 1;
			} );
			return chk;
		};
	}

	UIWidget* createCell( UIWidget* rowWidget, const ModelIndex& index ) {
		if ( index.column() == PluginsModel::Title ) {
			UITableCell* widget = UITableCell::NewWithOpt(
				mTag + "::cell", getCheckBoxFn( index, (const PluginsModel*)getModel() ) );
			return setupCell( widget, rowWidget, index );
		}
		return UITableView::createCell( rowWidget, index );
	}
};

UIWindow* UIPluginManager::New( UISceneNode* sceneNode, PluginManager* manager ) {
	if ( !UIWidgetCreator::isWidgetRegistered( "UIPluginManagerTable" ) )
		UIWidgetCreator::registerWidget( "UIPluginManagerTable",
										 [] { return eeNew( UIPluginManagerTable, () ); } );

	UIWindow* win = sceneNode
						->loadLayoutFromString( R"xml(
	<window
		id="plugin-manager-window"
		lw="800dp" lh="400dp"
		padding="8dp"
		window-title="Plugin Manager"
		window-flags="default|maximize|shadow"
		window-min-size="300dp 300dp">
		<vbox lw="mp" lh="mp">
			<UIPluginManagerTable id="plugin-manager-table" lw="mp" lh="fixed" layout_weight="1" />
			<vbox lw="mp" lh="wc">
				<hbox margin-top="4dp" layout-gravity="right">
					<!-- <pushbutton id="plugin-manager-preferences" enabled="false" text="Enabled" /> -->
					<pushbutton id="plugin-manager-close" text="Close" icon="close" margin-left="4dp" />
				</hbox>
			</vbox>
		</vbox>
	</window>
	)xml" )
						->asType<UIWindow>();
	UIWidget* cont = win->getContainer();
	UIPushButton* close = cont->find<UIPushButton>( "plugin-manager-close" );
	UIPluginManagerTable* tv =
		win->getContainer()->find<UIPluginManagerTable>( "plugin-manager-table" );
	close->addEventListener( Event::MouseClick, [win]( const Event* event ) {
		const MouseEvent* mevent = static_cast<const MouseEvent*>( event );
		if ( mevent->getFlags() & EE_BUTTON_LMASK )
			win->closeWindow();
	} );
	win->setTitle( sceneNode->i18n( "plugin_manager", "Plugin Manager" ) );
	tv->setModel( PluginsModel::New( manager ) );
	tv->setColumnsVisible(
		{ PluginsModel::Title, PluginsModel::Description, PluginsModel::Version } );
	tv->setAutoColumnsWidth( true );
	win->center();
	return win;
}

} // namespace ecode