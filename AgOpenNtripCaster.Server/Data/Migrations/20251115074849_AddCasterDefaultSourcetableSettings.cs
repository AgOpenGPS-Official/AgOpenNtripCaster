using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AgOpenNtripCaster.Server.Data.Migrations
{
    /// <inheritdoc />
    public partial class AddCasterDefaultSourcetableSettings : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "DefaultAuthentication",
                table: "CasterInfos",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<string>(
                name: "DefaultCompression",
                table: "CasterInfos",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<string>(
                name: "DefaultCountryCode",
                table: "CasterInfos",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<bool>(
                name: "DefaultFeeRequired",
                table: "CasterInfos",
                type: "boolean",
                nullable: false,
                defaultValue: false);

            migrationBuilder.AddColumn<string>(
                name: "DefaultGenerator",
                table: "CasterInfos",
                type: "text",
                nullable: false,
                defaultValue: "");

            migrationBuilder.AddColumn<string>(
                name: "DefaultNetwork",
                table: "CasterInfos",
                type: "text",
                nullable: false,
                defaultValue: "");
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "DefaultAuthentication",
                table: "CasterInfos");

            migrationBuilder.DropColumn(
                name: "DefaultCompression",
                table: "CasterInfos");

            migrationBuilder.DropColumn(
                name: "DefaultCountryCode",
                table: "CasterInfos");

            migrationBuilder.DropColumn(
                name: "DefaultFeeRequired",
                table: "CasterInfos");

            migrationBuilder.DropColumn(
                name: "DefaultGenerator",
                table: "CasterInfos");

            migrationBuilder.DropColumn(
                name: "DefaultNetwork",
                table: "CasterInfos");
        }
    }
}
