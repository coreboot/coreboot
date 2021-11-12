# Categories ![alt text](images/StarLabs_Logo.png "Star Labs Systems")

CMOS values should be added to [categories.yaml](src/application/categories.yaml].

This allows `coreboot-configurator` to display them in a relavant tab, with a nice
name and help text. Without this, they will still be visible in the **Raw** tab.

An example entry is below:
```
processor:
  displayName: Processor
  me_state:
    displayName: Intel Management Engine
    type: bool
    help: Enable or disable the Intel Management Engine
```

To explain the options:
```
**tabgroup**:				<- This is the reference to the tab group
  displayName: **Hello World**		<- This is the name of the group that the user
					   will see
  **setting_1**:			<- This is the value that should match the CMOS
					   option.
    displayName: **Hi World**		<- This is the name of the option that the user
					   will see.
    type: **bool**			<- Valid type are: bool (checkbox) and enum
					<- (dropdown).
    help: **Greet the World**		<- Help text that is displayed when hovering on the
					   option.
```
