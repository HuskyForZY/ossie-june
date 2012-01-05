<?xml version="1.0" encoding="UTF-8"?>
<softpkg id="DCE:82f6515a-de05-47f0-8e7a-1c9f621c00ee" name="DeviceManager">
    <author>
        <name>OSSIE</name>
        <company>Wireless@VT</company>
        <webpage>http://ossie.wireless.vt.edu</webpage>
    </author>
	<descriptor>
		<localfile name="DeviceManager.scd.xml"/>
	</descriptor>
	<implementation id="DCE:d3e8aba5-4421-45c5-9be6-372b763883e7">
		<description>implementation of a Device Manager</description>
		<code type="Executable">
			<localfile name="/domainmanager.exe"/>
			<entrypoint>domainmanager.exe</entrypoint>
		</code>
		<processor name="x86"/>
        <os name="Linux" version="2.6.26.3"/>
	</implementation>
</softpkg>
