DELETE FROM `spell_script_names` WHERE `spell_id` IN (62207, 63007, 62485, 65587, 62484, 65588, 62483, 65589);
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (62207, 'spell_elder_brightleaf_unstable_sun_beam');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (63006, 'spell_aggregation_pheromones_targeting');

-- TODO: I'm not sure about these spells, i.e. if the IDs are correct (I've taken them from script).
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (62485, 'spell_elder_brightleaf_essence_targeting');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (65587, 'spell_elder_brightleaf_essence_targeting');

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (62484, 'spell_elder_ironbranch_essence_targeting');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (65588, 'spell_elder_ironbranch_essence_targeting');

INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (62483, 'spell_elder_stonebark_essence_targeting');
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (65589, 'spell_elder_stonebark_essence_targeting');

-- Achievement stuff - again: due to DISTINCT(ScriptName) in ObjectMgr
UPDATE `achievement_criteria_data` SET `ScriptName`='achievement_getting_back_to_nature_25' WHERE `criteria_id`=10758 AND `type`=11;
UPDATE `achievement_criteria_data` SET `ScriptName`='achievement_knock_on_wood_25' WHERE `criteria_id`=10459 AND `type`=11;
UPDATE `achievement_criteria_data` SET `ScriptName`='achievement_knock_knock_on_wood_25' WHERE `criteria_id`=10460 AND `type`=11;
UPDATE `achievement_criteria_data` SET `ScriptName`='achievement_knock_knock_knock_on_wood_25' WHERE `criteria_id`=10461 AND `type`=11;